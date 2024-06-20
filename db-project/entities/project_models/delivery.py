from django.db import models


class Delivery(models.Model):
    storage = models.ForeignKey(
        'Storage',
        on_delete=models.CASCADE,
        verbose_name='Склад',
        help_text='Связь с главным складом'
    )

    vendor = models.ForeignKey(
        'Vendor',
        on_delete=models.CASCADE,
        verbose_name='Поставщик',
        help_text='Какой поставщик осуществляет эту поставку'
    )

    delivery_date = models.DateField(
        verbose_name='Дата поставки',
    )

    class Meta:
        db_table = 'deliveries'
        verbose_name = 'Поставка'
        verbose_name_plural = 'Поставки'
        db_table_comment = 'Поставки в главный склад'

    def __str__(self):
        return f'{self.storage} {self.vendor} {self.delivery_date}'
