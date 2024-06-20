import json
import traceback
import uuid

from django.contrib.auth.forms import AuthenticationForm
from django.contrib.auth.views import LoginView, LogoutView
from django.db import connection, transaction, InternalError
from django.http import JsonResponse
from django.shortcuts import render
from django.urls import reverse_lazy
from django.utils.decorators import method_decorator
from django.utils.translation import gettext_lazy as _
from django.views import View
from django.views.decorators.csrf import csrf_exempt

from entities.project_models import *
from queries.urls import QUERIES_NUMBER


def index(request):
    return render(request, 'pages/index.html')


def get_queries_list(request):
    n = QUERIES_NUMBER
    context = {
        'queries_size': range(1, n + 1),
    }
    return render(request, 'pages/queries.html', context=context)


def count_services_cost(service_elements, outlet_id):
    transformed_services = []
    for service in service_elements:
        transformed_service = {
            'service_type_id': int(service['option']),
            'amount': int(service['count'])
        }
        if 'dropdownValues' in service:
            transformed_service['codes'] = service['dropdownValues']
        transformed_services.append(transformed_service)

    transformed_elements = {
        'services': transformed_services,
        'outlet_id': outlet_id
    }

    service_elements_json = json.dumps(transformed_elements)

    with connection.cursor() as cursor:
        cursor.execute("SELECT calculate_service_type_cost(%s::jsonb)", [service_elements_json])
        result = cursor.fetchone()[0]
        return result


def count_items_cost(item_elements):
    transformed_items = []
    for item in item_elements:
        transformed_item = {
            'item_id': int(item['option']),
            'amount': int(item['count'])
        }
        transformed_items.append(transformed_item)

    item_elements_json = json.dumps(transformed_items)

    with connection.cursor() as cursor:
        cursor.execute("SELECT calculate_sale_orders_cost(%s::jsonb)", [item_elements_json])
        result = cursor.fetchone()[0]
        return result


def calculate_print_cost(print_items):
    transformed_prints = []
    for print_item in print_items:
        transformed_print = {
            'price_id': int(print_item['option']),
            'amount': int(print_item['count']),
        }
        transformed_prints.append(transformed_print)

    transformed_prints_json = json.dumps(transformed_prints)

    with connection.cursor() as cursor:
        cursor.execute("SELECT calculate_print_cost(%s::jsonb)", [transformed_prints_json])
        print_cost = cursor.fetchone()[0]

    return print_cost


def apply_discount(current_price, discount_percent):
    with connection.cursor() as cursor:
        cursor.execute("SELECT apply_discount(%s, %s)", (current_price, discount_percent))
        return cursor.fetchone()[0]


@csrf_exempt
def calculate_creating_order(request):
    # Ensure the request method is POST
    if request.method != 'POST':
        return JsonResponse({'error': 'Invalid HTTP method'}, status=405)

    # Parse the request body to get the data
    try:
        data = json.loads(request.body)
    except json.JSONDecodeError:
        return JsonResponse({'error': 'Invalid JSON'}, status=400)

    # Calculate the costs
    services_cost = count_services_cost(data['services'], 1)
    items_cost = count_items_cost(data['items'])
    print_cost = calculate_print_cost(data['print'])

    cur_price = services_cost + items_cost + print_cost
    if data['urgency']:
        cur_price *= 2

    client_id = data['client_id']
    if client_id != '':
        client = Client.objects.get(id=int(data['client_id']))
        cur_price = apply_discount(cur_price, client.discount)

    response = {
        "services_cost": services_cost,
        "items_cost": items_cost,
        "print_cost": print_cost,
        'general_cost': cur_price
    }

    # Return the response as JSON
    return JsonResponse(response)


@method_decorator(csrf_exempt, name='dispatch')
class CreateOrder(View):

    @staticmethod
    def get(request):
        outlet_id = getattr(request.user, 'outlet_id', 1)
        storage_items = StorageItem.objects.filter(storage__outlet_id=outlet_id).select_related('item')
        print_prices = PrintPrice.objects.all()

        service_types = ServiceType.objects.filter(
            servicetypeoutlet__outlet_type__outlet__id=outlet_id
        ).distinct()

        context = {
            'clients': Client.objects.all(),
            'services': service_types,
            'storage_items': storage_items,
            'print_prices': print_prices,
        }

        return render(request, 'pages/create_order.html', context=context)

    @staticmethod
    def post(request):
        outlet_id = getattr(request.user, 'outlet_id', 1)
        data = json.loads(request.body)
        print(data)
        try:
            if data['client_id'] == '':
                raise ValueError('Укажите клиента')

            with transaction.atomic():
                order = Order.objects.create(
                    client=Client.objects.get(id=data['client_id']),
                    accept_outlet=Outlet.objects.get(id=outlet_id),
                    is_urgent=data['urgency']
                )
                print(order)
                services = data['services']
                for service in services:
                    service_order = ServiceOrder.objects.create(
                        order=order,
                        service_type=ServiceType.objects.get(id=service['option']),
                        count=int(service['count'])
                    )
                    if 'dropdownValues' in service:
                        for c in service['dropdownValues']:
                            Film.objects.create(
                                code=c,
                                service_order=service_order
                            )

                items = data['items']

                for item in items:
                    SaleOrder.objects.create(
                        order=order,
                        item=Item.objects.get(id=item['option']),
                        amount=int(item['count'])
                    )

                frames = data['print']
                if len(frames) > 0:
                    print_order = PrintOrder.objects.create(order=order)
                    for frame in frames:
                        Frame.objects.create(
                            print_order=print_order,
                            amount=int(frame['count']),
                            print_price=PrintPrice.objects.get(id=frame['option']),
                            frame_number=int(frame['customFields']['frame_number']),
                            code=uuid.uuid4()
                        )

        except InternalError as e:
            err = str(e)
            res_err = err.split('CONTEXT:')[0].strip()
            print(res_err)
            return JsonResponse({'error': res_err})
        except Exception as e:
            print(e)
            print(traceback.format_exc())

            return JsonResponse({'error': str(e)})

        return JsonResponse({'success': True})


class CustomAuthenticationForm(AuthenticationForm):
    error_messages = {
        'invalid_login': _(
            "Логин или пароль неверный."
        ),
        'inactive': _("Этот аккаунт заблокирован"),
    }


class LoginPageView(LoginView):
    template_name = 'pages/login.html'
    authentication_form = CustomAuthenticationForm

    redirect_authenticated_user = True

    def get_success_url(self):
        return reverse_lazy('index')


class LogoutPageView(LogoutView):
    next_page = reverse_lazy('index')
