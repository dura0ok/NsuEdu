from django.db import models
from django.db.models import UniqueConstraint

from entities.project_models.firm import Firm


class Item(models.Model):
    firm = models.ForeignKey(
        Firm,
        on_delete=models.CASCADE,
        db_comment='Связь с брэндом',
        verbose_name='Бренд',
        help_text='Выберите бренд товара'
    )
    product_name = models.CharField(
        max_length=100,
        db_comment='Название товара',
        verbose_name='Наименование',
        help_text='Введите название товара'
    )
    price = models.DecimalField(
        max_digits=15,
        decimal_places=2,
        db_comment='Цена',
        help_text='Введите цену товара'
    )

    class Meta:
        db_table = 'items'
        constraints = [
            UniqueConstraint(
                fields=['firm', 'product_name'],
                name='unique_item_firm_product_name')
        ]
        db_table_comment = 'Товары'
        verbose_name = 'Товар'
        verbose_name_plural = 'Товары'

    def __str__(self):
        return f'{self.firm} - {self.product_name}'
