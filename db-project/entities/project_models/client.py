from django.core.validators import MaxValueValidator
from django.db import models


class Client(models.Model):
    full_name = models.CharField(
        verbose_name='Фио',
        max_length=50,
        db_comment='ФИО клиента',
        help_text='Введите ФИО клиента.',
    )

    is_professional = models.BooleanField(
        verbose_name='Профессионализм',
        db_comment='Профессионал или Любитель?',
        help_text='Отметьте, если клиент является профессионалом.',
        default=False
    )

    discount = models.PositiveSmallIntegerField(
        db_comment='Персональная скидка',
        verbose_name='Персональная скидка',
        help_text='Введите персональную скидку клиента.',
        validators=[MaxValueValidator(100)]
    )

    class Meta:
        verbose_name = 'Клиент(а)'
        verbose_name_plural = 'Клиенты'
        db_table = 'clients'
        db_table_comment = 'Таблица клиентов различных фотоцентров'

    def __str__(self):
        return self.full_name
