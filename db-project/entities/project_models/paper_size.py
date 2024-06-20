from django.db import models


class PaperSize(models.Model):
    name = models.CharField(
        unique=True,
        max_length=30,
        verbose_name='Наименование',
        help_text='Название размера бумаги'
    )

    class Meta:
        verbose_name = 'Размер бумаги'
        verbose_name_plural = 'Размеры бумаги'
        db_table = 'paper_sizes'
        db_table_comment = 'Формат бумаги'

    def __str__(self):
        return f'{self.name}'
