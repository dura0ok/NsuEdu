from django.urls import path

from .views import *

unused()

QUERIES_NUMBER = 12
urlpatterns = []

for i in range(1, QUERIES_NUMBER + 1):
    handler = getattr(locals()[f"Query{i}Handler"], 'as_view')
    urlpatterns.append(path(str(i), handler()))
