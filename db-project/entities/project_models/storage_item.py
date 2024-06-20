from django.core.validators import MinValueValidator
from django.db import models


class StorageItem(models.Model):
    storage = models.ForeignKey(
        'Storage',
        verbose_name='Склад',
        on_delete=models.CASCADE,
        help_text='Связь со складом зданий'
    )
    item = models.ForeignKey(
        'Item',
        verbose_name='Товар',
        on_delete=models.CASCADE,
        help_text='Связь с товаром'
    )

    quantity = models.PositiveSmallIntegerField(
        verbose_name='Количество',
        help_text='Количество товаров данного типа на складе',
        validators=[MinValueValidator(1)]
    )

    class Meta:
        verbose_name = 'Товар на складе'
        verbose_name_plural = 'Товары на складах'
        db_table = 'storage_items'
        constraints = [
            models.UniqueConstraint(
                fields=['storage', 'item'],
                name='storage_items_unique'
            )
        ]
        db_table_comment = 'Товары на складах зданий'

    def __str__(self):
        return f'{self.storage} {self.item} - {self.quantity}'
