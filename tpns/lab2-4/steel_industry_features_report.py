import pandas as pd
from ucimlrepo import fetch_ucirepo

from helpers import gain_ratio, gain_ratio_regression
from task3_loader import task3_loader


def main():
    target = "Load_Type"
    df = task3_loader()
    encoded_data = pd.get_dummies(df, columns=['WeekStatus', 'Day_of_week'])
    res = gain_ratio(encoded_data, target_col=target, category_col=target)
    sorted_data = dict(sorted(res.items(), key=lambda el: el[1][1], reverse=True))
    print(sorted_data)
    get_features = list(sorted_data.keys())[:5]
    print(get_features)


if __name__ == "__main__":
    main()
