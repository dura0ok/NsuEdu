from django.db import models
from django.utils import timezone


class Order(models.Model):
    client = models.ForeignKey(
        'Client',
        on_delete=models.CASCADE,
        verbose_name='Клиент',
        help_text='Клиент, который сделал заказ',
        db_comment='Клиент, который сделал заказ'
    )
    accept_outlet = models.ForeignKey(
        'Outlet',
        on_delete=models.CASCADE,
        verbose_name='Адрес',
        help_text='Где приняли заказ, адрес(здание)',
        db_comment='Где приняли заказ'
    )

    accept_timestamp = models.DateTimeField(
        default=timezone.now,
        verbose_name='Время',
        help_text='Когда заказ поступил',
        db_comment='Когда заказ поступил'
    )

    total_amount_price = models.DecimalField(
        verbose_name='Суммарный чек',
        help_text='Суммарная цена заказа, которая рассчитывается из купленных товаров, услуг....',
        db_comment='Суммарная цена заказа, которая рассчитывается из купленных товаров, услуг....',
        default=0,
        max_digits=10,
        decimal_places=2,
    )

    is_urgent = models.BooleanField(
        verbose_name='Срочность',
        help_text='Срочность заказа',
        db_comment='Срочность заказа',
        default=False
    )

    class Meta:
        db_table = 'orders'
        db_table_comment = 'Заказы'
        verbose_name = 'Заказ'
        verbose_name_plural = 'Заказы'

    def __str__(self):
        return f'{self.client} - {self.accept_outlet.address} -{self.accept_timestamp}'
