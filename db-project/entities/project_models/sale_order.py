from django.db import models
from django.db.models import UniqueConstraint

from entities.project_models import Order, Item


class SaleOrder(models.Model):
    order = models.ForeignKey(
        Order,
        on_delete=models.CASCADE,
        verbose_name='Заказ',
        help_text='Связь с заказом',
        db_comment='Связь с заказом'
    )

    item = models.ForeignKey(
        Item,
        verbose_name='Товар',
        on_delete=models.CASCADE,
        help_text='Товар, который продаем',
        db_comment='Связь с товарами'
    )

    amount = models.PositiveSmallIntegerField(
        verbose_name='Количество',
        db_comment='Количество'
    )

    class Meta:
        db_table = 'sale_orders'
        db_table_comment = 'Продажи товаров в заказе'
        verbose_name = 'Продажа товара в заказе'
        verbose_name_plural = 'Продажи товаров в заказе'

        constraints = [
            UniqueConstraint(
                fields=['order', 'item'],
                name='unique_sale_order_item')
        ]

    def __str__(self):
        return f'{self.order} {self.item} {self.amount}'
