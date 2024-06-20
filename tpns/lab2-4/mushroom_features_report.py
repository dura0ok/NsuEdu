import pandas as pd
from matplotlib import pyplot as plt
from sklearn.preprocessing import OneHotEncoder, LabelEncoder
from ucimlrepo import fetch_ucirepo
import seaborn as sns

from helpers import gain_ratio


def main():
    # fetch dataset
    mushroom = fetch_ucirepo(id=73)

    # data (as pandas dataframes)
    x = mushroom.data.features
    y = mushroom.data.targets

    df = pd.concat([x, y], axis=1)
    label_encoder = LabelEncoder()
    for col in df.columns:
        if df[col].dtype == 'object':
            df[col] = label_encoder.fit_transform(df[col])
    plt.figure(figsize=(8, 6))
    sns.heatmap(df.corr(), annot=False, cmap='coolwarm', fmt=".2f")
    plt.show()

    target = y.columns[0]
    correlations = df.corr()[target].abs()
    threshold = 0.3

    selected_features = correlations[correlations > threshold].index.tolist()
    print(selected_features)

    selected_data = df[selected_features]
    print(selected_data.columns)

    res = gain_ratio(df, target_col=target, category_col=target)
    sorted_data = dict(sorted(res.items(), key=lambda el: el[1][1], reverse=True))
    get_features = list(sorted_data.keys())[:3]
    print(get_features)


if __name__ == '__main__':
    main()
