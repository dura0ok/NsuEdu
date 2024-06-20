import datetime
from datetime import date
from typing import Optional

from django.db import connection
from django.http import JsonResponse

from queries.entities import *


def _generate_columns_list(cols: list[str]) -> list[dict[str, str]]:
    return [{"title": col, "field": col} for col in cols]


def execute_query(query, params=None, *args: str) -> QueryResult:
    custom_column_names: list[str] = list(args)
    with connection.cursor() as cursor:
        cursor.execute(query, params)
        columns = _generate_columns_list(custom_column_names) if args else _generate_columns_list(
            [returned_column.name for returned_column in cursor.description]
        )
        data = cursor.fetchall()
        data_with_field_keys = [{col["field"]: row[i] for i, col in enumerate(columns)} for row in data]

        return QueryResult(columns=columns, data=data_with_field_keys)


def build_success_response(responses: list[QueryResult]) -> JsonResponse:
    results = []
    for index, response in enumerate(responses):
        results.append({'columns': response.columns, 'data': response.data})

    return JsonResponse({'results': results}, safe=False,
                        json_dumps_params={'ensure_ascii': False})


def build_error_response(message: str) -> JsonResponse:
    return JsonResponse({'error': True, 'message': message}, safe=False, json_dumps_params={'ensure_ascii': False})


def validate_date_range(start: Optional[str], end: Optional[str]) -> tuple[date, date]:
    if not (start and end):
        raise ValueError("Даты должны быть заданы")
    try:
        start_date: datetime.date = datetime.datetime.strptime(start, '%Y-%m-%d').date()
        end_date: datetime.date = datetime.datetime.strptime(end, '%Y-%m-%d').date()
    except ValueError:
        raise ValueError('Ошибка формата даты')
    if start_date > end_date:
        raise ValueError('Дата начала должна быть раньше конца')
    return start_date, end_date
