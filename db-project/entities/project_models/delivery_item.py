from django.db import models
from django.db.models import UniqueConstraint


class DeliveryItem(models.Model):
    delivery = models.ForeignKey(
        'Delivery',
        models.CASCADE,
        help_text='Связь с поставкой',
    )
    item = models.ForeignKey(
        'Item',
        models.CASCADE,
        help_text='Товар в поставке'
    )
    purchase_price = models.DecimalField(
        max_digits=15,
        decimal_places=2,
        db_comment='Цена закупки товара',
        help_text='Цена по которой закупалась'
    )
    amount = models.IntegerField(
        db_comment='Количество поставленных единиц товара',
        help_text='Кол-во товара в поставке'
    )

    class Meta:
        db_table = 'delivery_items'
        constraints = [
            UniqueConstraint(
                fields=['delivery', 'item'],
                name='delivery_items_unique')
        ]
        db_table_comment = 'Товары в поставке'
        verbose_name = 'Товар в поставке'
        verbose_name_plural = 'Товары в поставках'

    def __str__(self):
        return f'{self.delivery} - {self.item}'
