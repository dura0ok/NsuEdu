from django.core.validators import MinValueValidator
from django.db import models


class VendorItem(models.Model):
    vendor = models.ForeignKey(
        'Vendor',
        on_delete=models.CASCADE,
        verbose_name='Поставщик',
        db_comment='Связь с поставщиком'
    )
    item = models.ForeignKey(
        'Item',
        on_delete=models.CASCADE,
        verbose_name='Предмет',
        db_comment='Связь с товаром'
    )

    current_price = models.PositiveSmallIntegerField(
        verbose_name='Цена',
        db_comment='Текущая цена по которой продает поставщик данный товар'
    )

    quantity = models.PositiveSmallIntegerField(
        verbose_name='Количество',
        db_comment='Текущая цена по которой продает поставщик данный товар',
        validators=[MinValueValidator(1)],
    )

    class Meta:
        db_table = 'vendor_items'
        constraints = [
            models.UniqueConstraint(
                fields=['vendor', 'item'],
                name='vendor_items_unique'
            )
        ]

        verbose_name = 'Товар поставщика'
        verbose_name_plural = 'Товары поставщика'
        db_table_comment = 'По какой цене продает поставщик товары'

    def __str__(self):
        return f'{self.vendor} - ({self.item})'
