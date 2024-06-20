from django.contrib import admin
from django.contrib.auth.admin import UserAdmin

from entities.models import CustomUser
from entities.project_models import *


class CustomUserAdmin(UserAdmin):
    # Add the new field to fieldsets to make it editable in the admin
    fieldsets = UserAdmin.fieldsets + (
        (None, {'fields': ('outlet',)}),
    )

    add_fieldsets = UserAdmin.add_fieldsets + (
        (None, {'fields': ('outlet',)}),
    )
    list_display = UserAdmin.list_display + ('outlet',)


# Register the custom user admin
admin.site.register(CustomUser, CustomUserAdmin)
admin.site.register(Client)
admin.site.register(OutletType)
admin.site.register(Outlet)
admin.site.register(Branch)
admin.site.register(Kiosk)
admin.site.register(PhotoStore)
admin.site.register(Firm)
admin.site.register(Item)
admin.site.register(VendorItem)
admin.site.register(Vendor)
admin.site.register(Storage)
admin.site.register(StorageItem)
admin.site.register(PaperType)
admin.site.register(PaperSize)
admin.site.register(PrintDiscount)
admin.site.register(PrintPrice)
admin.site.register(Film)
admin.site.register(Frame)
admin.site.register(PrintOrder)
admin.site.register(SaleOrder)
admin.site.register(Order)
admin.site.register(Delivery)
admin.site.register(DeliveryItem)
admin.site.register(FilmDevelopmentOrder)
admin.site.register(ServiceType)
admin.site.register(ServiceOrder)
admin.site.register(ServiceTypeOutlet)
admin.site.register(ServiceTypeNeededItem)
