from django.db import models

from entities.project_models import Outlet


class PhotoStore(models.Model):
    outlet = models.OneToOneField(
        Outlet,
        on_delete=models.CASCADE,
        primary_key=True,
        verbose_name='Адрес',
        db_comment='Привязка к зданию',
        help_text='Связь со зданием в котором расположен фотомагазин'
    )

    class Meta:
        db_table = 'photo_stores'
        db_table_comment = 'Фотомагазины'
        verbose_name = 'Фотомагазин'
        verbose_name_plural = 'Фотомагазины'

    def __str__(self):
        return f'{self.outlet.address}'
