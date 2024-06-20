from django.db.models import Q
from django.shortcuts import render
from django.views import View

from entities.project_models import OutletType, Outlet, Item
from .helpers import *


def unused():
    pass


def get_outlet_types():
    outlet_types = OutletType.objects.filter(
        Q(name='Филиал') | Q(name='Киоск') | Q(name='Фотомагазин')
    )
    return {'outlet_types': outlet_types}


def get_outlets():
    outlets = Outlet.objects.select_related('type').filter(
        Q(type__name='Филиал') | Q(type__name='Киоск') | Q(type__name='Фотомагазин')
    ).order_by('id')
    return {'outlets': outlets}


class Query1Handler(View):
    @staticmethod
    def get(request):
        context = get_outlet_types()
        return render(request, 'pages/queries/1.html', context=context)

    @staticmethod
    def post(request):
        outlet_type_id = request.POST.get('outlet')
        query = '''
                  SELECT o.address, o.num_workers 
                  FROM outlets o 
                  JOIN outlet_types t ON o.type_id = t.id 
                  WHERE o.type_id = %s
                  '''
        return build_success_response([execute_query(query, (outlet_type_id,), 'Адрес', 'Количество работников')])


class Query2Handler(View):
    @staticmethod
    def get(request):
        context = get_outlet_types()
        return render(request, 'pages/queries/2.html', context=context)

    @staticmethod
    def post(request):
        outlet_type_id = request.POST.get('outlet_type')
        start = request.POST.get('start_date')
        end = request.POST.get('end_date')
        try:
            start, end = validate_date_range(start, end)
        except ValueError as e:
            return build_error_response(str(e))

        query = '''
               SELECT c.full_name,
                      ot.address,
                      TO_CHAR(o.accept_timestamp, 'DD-MM-YYYY HH24:MI:SS') AS accept_timestamp,
                      ot_types.name AS accept_type
               FROM public.orders o
               JOIN public.outlets ot ON o.accept_outlet_id = ot.id
               JOIN public.outlet_types ot_types ON ot.type_id = ot_types.id
               JOIN clients c ON o.client_id = c.id
               WHERE ot.type_id = %s AND o.accept_timestamp BETWEEN %s AND %s
               '''

        return build_success_response(
            [
                execute_query(
                    query,
                    (outlet_type_id, start, end),
                    'ФИО', 'Адрес', 'Время приёма', 'Тип здания'
                )
            ]
        )


class Query3Handler(View):
    @staticmethod
    def get(request):
        context = get_outlets()
        return render(request, 'pages/queries/3.html', context=context)

    @staticmethod
    def post(request):
        outlet_type_id = request.POST.get('outlet')
        urgency = request.POST.get('urgency')
        start = request.POST.get('start_date')
        end = request.POST.get('end_date')

        try:
            start, end = validate_date_range(start, end)
        except ValueError as e:
            return build_error_response(str(e))

        query = '''
                    SELECT 
                        st.name,
                        CASE WHEN o.is_urgent THEN 'Срочный' ELSE 'Не срочный' END AS is_urgent,
                        TO_CHAR(o.accept_timestamp, 'DD-MM-YYYY HH24:MI:SS') AS accept_timestamp,
                        c.full_name
                    FROM 
                        service_orders so
                    JOIN 
                        orders o ON so.order_id = o.id
                    JOIN 
                        service_types st ON so.service_type_id = st.id
                    JOIN 
                        public.clients c ON o.client_id = c.id
                    JOIN 
                        outlets ot ON o.accept_outlet_id = ot.id
                    WHERE 
                        accept_outlet_id = %s 
                        AND is_urgent = %s
                        AND accept_timestamp BETWEEN %s AND %s;
                '''
        resp = execute_query(query, (outlet_type_id, urgency, start, end), 'Название услуги', 'Срочность', 'Время приёма', 'Имя Клиента')
        resp.add_value_to_column_by_field(field_name="Время приёма", key="bottomCalc", data="count")
        return build_success_response([resp])


class Query4Handler(View):
    @staticmethod
    def get(request):
        context = get_outlets()
        return render(request, 'pages/queries/4.html', context=context)

    @staticmethod
    def post(request):
        outlet_id = request.POST.get('outlet')
        urgency = request.POST.get('urgency')
        start = request.POST.get('start_date')
        end = request.POST.get('end_date')

        try:
            start, end = validate_date_range(start, end)
        except ValueError as e:
            return build_error_response(str(e))
        query = '''
            SELECT ot.address, c.full_name, 
            TO_CHAR(o.accept_timestamp, 'DD-MM-YYYY HH24:MI:SS'), calculate_service_orders_price(o.id)
            FROM   orders o
            JOIN public.clients c on c.id = o.client_id
            JOIN outlets ot ON o.accept_outlet_id = ot.id
            WHERE  accept_outlet_id = %s
                   AND is_urgent = %s
                   AND accept_timestamp BETWEEN %s AND %s; ;
        '''
        resp = execute_query(query, (outlet_id, urgency, start, end), 'Адрес', 'Фио Клиента', 'Время',
                             'Стоимость услуг')
        resp.add_value_to_column_by_field(field_name="Стоимость услуг", key="bottomCalc", data="sum")
        return build_success_response([resp])


class Query5Handler(View):
    @staticmethod
    def get(request):
        context = get_outlets()
        context["all_outlets"] = True
        return render(request, 'pages/queries/5.html', context=context)

    @staticmethod
    def post(request):
        outlet_id = request.POST.get('outlet')
        urgency = request.POST.get('urgency')
        start = request.POST.get('start_date')
        end = request.POST.get('end_date')

        try:
            start, end = validate_date_range(start, end)
        except ValueError as e:
            return build_error_response(str(e))

        query = f'''
                    SELECT
                    ot.address,
                    c.full_name,
                    TO_CHAR(o.accept_timestamp, 'DD-MM-YYYY HH24:MI:SS'),
                    COALESCE(SUM(f.amount), 0)
                FROM
                    print_orders po
                JOIN 
                    orders o ON po.order_id = o.id
                JOIN
                    public.clients c ON c.id = o.client_id
                JOIN
                    outlets ot ON o.accept_outlet_id = ot.id
                LEFT JOIN
                    public.frames f ON po.id = f.print_order_id
                WHERE
                    o.is_urgent = %s
                    AND o.accept_timestamp BETWEEN %s AND %s 
                    {'AND o.accept_outlet_id = %s' if outlet_id else ''}
                    GROUP BY ot.address, c.full_name, o.accept_timestamp;
                '''

        args = [urgency, start, end]

        if outlet_id:
            args.append(outlet_id)

        resp = execute_query(
            query,
            args,
            'Адрес', 'ФИО', 'Дата', 'Количество'
        )

        resp.add_value_to_column_by_field(field_name="Количество", key="bottomCalc", data="sum")
        return build_success_response([
            resp
        ])


class Query6Handler(View):
    @staticmethod
    def get(request):
        context = get_outlets()
        context["all_outlets"] = True
        return render(request, 'pages/queries/6.html', context=context)

    @staticmethod
    def post(request):
        outlet_id = request.POST.get('outlet')
        urgency = request.POST.get('urgency')
        start = request.POST.get('start_date')
        end = request.POST.get('end_date')

        try:
            start, end = validate_date_range(start, end)
        except ValueError as e:
            return build_error_response(str(e))

        query = f'''
                        SELECT COUNT(*) FROM film_development_orders;
                        '''

        args = [urgency, start, end]

        if outlet_id:
            args.append(outlet_id)

        resp = execute_query(
            query,
            args,
            'Количество'
        )

        return build_success_response([
            resp
        ])


class Query7Handler(View):
    @staticmethod
    def get(request):
        items = Item.objects.all()
        context = {
            'items': items
        }
        return render(request, 'pages/queries/7.html', context=context)

    @staticmethod
    def post(request):
        item_id = request.POST.get("item")
        start = request.POST.get('start_date')
        end = request.POST.get('end_date')
        volume = request.POST.get('volume')

        if not volume:
            volume = 0

        try:
            start, end = validate_date_range(start, end)
        except ValueError as e:
            return build_error_response(str(e))

        args = [start, end, volume]

        if item_id:
            args.append(item_id)

        query = f'''
                    SELECT
                    v.name,
            SUM(di.amount) AS delivery_count
        FROM
            vendors v
        JOIN vendor_items vi ON v.id = vi.vendor_id
        JOIN delivery_items di ON vi.item_id = di.item_id
        JOIN deliveries d ON di.delivery_id = d.id
        WHERE
            d.delivery_date BETWEEN %s AND %s
            AND di.amount >= %s
            {'AND vi.item_id = %s' if item_id else ''}
        GROUP BY
            v.name;
        '''

        resp = execute_query(query, args, 'Имя поставщика', 'Количество поставок')
        resp.add_value_to_column_by_field(field_name="Количество поставок", key="bottomCalc", data="sum")
        return build_success_response([resp])


class Query8Handler(View):
    @staticmethod
    def get(request):
        context = get_outlets()
        context["all_outlets"] = True
        return render(request, 'pages/queries/8.html', context=context)

    @staticmethod
    def post(request):
        outlet_id = request.POST.get("outlet")
        buy_min = request.POST.get('volume')
        discount_min = request.POST.get('discount')

        args = [discount_min, buy_min]

        if outlet_id:
            args.append(outlet_id)

        resp1 = execute_query(
            f'''
                            SELECT DISTINCT  c.full_name, 
                               CASE 
                                   WHEN is_professional THEN 'Да' 
                                   WHEN NOT is_professional THEN 'Нет' 
                               END AS is_professional, 
                               discount,
                               SUM(COALESCE(o.total_amount_price, 0))
                        FROM clients c
                        LEFT JOIN orders o on c.id = o.client_id
                        WHERE c.discount >= %s
                        GROUP BY c.id, c.full_name, c.is_professional, c.discount, total_amount_price, accept_outlet_id
                        HAVING SUM(COALESCE(o.total_amount_price, 0)) >= %s
                        {'AND o.accept_outlet_id = %s' if outlet_id else ''}
                        ''',
            args, 'Имя', 'Профессионал', 'Скидка', 'Обьем покупок')
        return build_success_response([resp1])


class Query9Handler(View):
    @staticmethod
    def get(request):
        context = get_outlets()
        context["all_outlets"] = True
        return render(request, 'pages/queries/9.html', context=context)

    @staticmethod
    def post(request):
        outlet_id = request.POST.get("outlet")
        start_date = request.POST.get("start_date")
        end_date = request.POST.get("end_date")

        try:
            start, end = validate_date_range(start_date, end_date)
        except ValueError as e:
            return build_error_response(str(e))

        args = [start, end]

        if outlet_id:
            args.append(outlet_id)

        query = f'''
            SELECT ot.address, c.full_name, 
                TO_CHAR(o.accept_timestamp, 'DD-MM-YYYY HH24:MI:SS'), calculate_sale_orders_price(o.id) 
                FROM orders o
                JOIN public.clients c on c.id = o.client_id
                JOIN outlets ot ON o.accept_outlet_id = ot.id
                WHERE accept_timestamp BETWEEN %s AND %s
                {'AND o.accept_outlet_id = %s' if outlet_id else ''}
        '''

        resp = execute_query(query, args, 'Адрес', 'ФИО', 'Дата', 'Цена проданных фототоваров в заказе')
        resp.add_value_to_column_by_field(field_name="Цена проданных фототоваров в заказе", key="bottomCalc",
                                          data="sum")
        return build_success_response([resp])


class Query10Handler(View):
    @staticmethod
    def get(request):
        context = get_outlets()
        context["all_outlets"] = True
        return render(request, 'pages/queries/10.html', context=context)

    @staticmethod
    def post(request):
        outlet_id = request.POST.get("outlet")

        query = f'''
            WITH SalesData AS (
                SELECT
                    f.name AS firm_name,
                    i.product_name AS product_name,
                    SUM(so.amount) AS count_saled,
                    ROW_NUMBER() OVER (ORDER BY SUM(so.amount) DESC) AS sales_rank
                FROM
                    firms f
                JOIN
                    items i ON i.firm_id = f.id
                JOIN
                    sale_orders so ON so.item_id = i.id
                JOIN
                    orders o ON so.order_id = o.id
                {'WHERE o.accept_outlet_id = %s' if outlet_id else ''}
                GROUP BY
                    f.name, i.product_name
            )
            
            SELECT
                firm_name,
                product_name,
                count_saled,
                sales_rank
            FROM
                SalesData
            ORDER BY
                sales_rank;
        '''

        resp = execute_query(query, outlet_id, 'Фирма', 'Продукт', 'Сколько продано', 'Место')
        return build_success_response([resp])


class Query11Handler(View):
    @staticmethod
    def get(request):
        context = get_outlets()
        context["all_outlets"] = True
        return render(request, 'pages/queries/11.html', context=context)

    @staticmethod
    def post(request):
        outlet_id = request.POST.get("outlet")
        start_date = request.POST.get("start_date")
        end_date = request.POST.get("end_date")

        try:
            start, end = validate_date_range(start_date, end_date)
        except ValueError as e:
            return build_error_response(str(e))

        args = [start, end]

        if outlet_id:
            args.append(outlet_id)

        query = f'''
            SELECT ot.address, c.full_name, 
                TO_CHAR(o.accept_timestamp, 'DD-MM-YYYY HH24:MI:SS'), calculate_sale_orders_price(o.id) 
                FROM orders o
                JOIN public.clients c on c.id = o.client_id
                JOIN outlets ot ON o.accept_outlet_id = ot.id
                WHERE accept_timestamp BETWEEN %s AND %s
                {'AND o.accept_outlet_id = %s' if outlet_id else ''}
        '''

        resp = execute_query(query, args, 'Адрес', 'ФИО', 'Дата', 'Цена проданных фототоваров в заказе')
        resp.add_value_to_column_by_field(field_name="Цена проданных фототоваров в заказе", key="bottomCalc",
                                          data="sum")
        return build_success_response([resp])


class Query12Handler(View):
    @staticmethod
    def get(request):
        context = get_outlet_types()
        context["all_types"] = True
        return render(request, 'pages/queries/12.html', context=context)

    @staticmethod
    def post(request):
        outlet_type_id = request.POST.get('outlet_type')
        print(outlet_type_id)
        if not outlet_type_id:
            query = '''
                SELECT
                  SUM(num_workers) AS total_workers
                FROM
                  outlets;
            '''
            return build_success_response([execute_query(query, None, 'Количество')])

        query = '''
                  SELECT o.address, o.num_workers 
                  FROM outlets o 
                  JOIN outlet_types t ON o.type_id = t.id 
                  WHERE o.type_id = %s
                  '''

        return build_success_response([execute_query(query, (outlet_type_id,), 'Адрес', 'Количество работников')])
