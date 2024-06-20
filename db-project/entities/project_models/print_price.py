from django.db import models
from django.db.models import UniqueConstraint

from entities.project_models import PaperSize, PaperType


class PrintPrice(models.Model):
    paper_size = models.ForeignKey(
        PaperSize,
        on_delete=models.CASCADE,
        verbose_name='Размер',
        help_text='Выберите формат бумаги',
        db_comment='Формат бумаги'
    )
    paper_type = models.ForeignKey(
        PaperType,
        verbose_name='Тип',
        on_delete=models.CASCADE,
        help_text='Выберите тип бумаги',
        db_comment='Тип бумаги'
    )

    price = models.PositiveSmallIntegerField(
        verbose_name='Цена',
        help_text='Укажите цену за (формат, печать)',
        db_comment='Цена за (формат, печать)'
    )

    class Meta:
        db_table = 'print_prices'
        constraints = [
            UniqueConstraint(
                fields=['paper_size', 'paper_type'],
                name='unique_print_price')
        ]
        db_table_comment = 'Расценки на печать'
        verbose_name = 'Расценка на печать'
        verbose_name_plural = 'Расценки на печать'

    def __str__(self):
        return f'{self.paper_type.name} - {self.paper_size}'
