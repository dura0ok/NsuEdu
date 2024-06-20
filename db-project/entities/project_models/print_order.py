from django.db import models


class PrintOrder(models.Model):
    order = models.ForeignKey(
        'Order',
        on_delete=models.CASCADE,
        verbose_name='Заказ',
        help_text='Связь с заказом',
        db_comment='Связь с заказом'
    )

    discount = models.ForeignKey(
        'PrintDiscount',
        on_delete=models.CASCADE,
        help_text='Скидка',
        db_comment='Скидка',
        null=True,
        blank=True
    )

    class Meta:
        db_table = 'print_orders'
        db_table_comment = 'Заказы на печать'
        verbose_name = 'Заказ на печать'
        verbose_name_plural = 'Заказы на печать'

    def __str__(self):
        return f'{self.order} - {self.discount}'
