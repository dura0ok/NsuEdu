from django.db import models


class Branch(models.Model):
    outlet = models.OneToOneField(
        'Outlet',
        on_delete=models.CASCADE,
        verbose_name='Адрес',
        primary_key=True,
        db_comment='Привязка к зданию',
        help_text='Связь со зданием в котором расположен филиал'
    )

    class Meta:
        db_table = 'branches'
        db_table_comment = 'Филиалы'
        verbose_name = 'Филиал'
        verbose_name_plural = 'Филиалы'

    def __str__(self):
        return f'{self.outlet.address}'
