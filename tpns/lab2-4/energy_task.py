import os

import numpy as np
import pandas as pd

from ucimlrepo import fetch_ucirepo

from mlp import MLP
from helpers import custom_train_test_split, create_sequence_data, DataProcessor


def load_x_y(drop_prev_load_type=True):
    data_dir = "data"
    if not os.path.exists(data_dir):
        os.makedirs(data_dir)

    data_path = os.path.join(data_dir, "energy_consumption.csv")
    try:
        df = pd.read_csv(data_path)
    except FileNotFoundError:
        print("Downloading data from UCI Repository...")
        steel_industry_energy_consumption = fetch_ucirepo(id=851)
        steel_industry_energy_consumption.data.original.to_csv(data_path, index=False)
        df = pd.read_csv(data_path)

    df["Prev_Load_Type"] = df["Load_Type"].shift(1)
    df = df.drop(df.index[0])

    one_hot_features = pd.get_dummies(df[["Day_of_week", "WeekStatus", "Prev_Load_Type"]], dtype=np.int64, prefix="")
    df = pd.concat([df, one_hot_features], axis=1)

    df = df.drop(["Prev_Load_Type"], axis=1)

    cols_to_scale = [
        "Usage_kWh",
        "Leading_Current_Reactive_Power_kVarh",
        "Lagging_Current_Reactive.Power_kVarh",
        "CO2(tCO2)",
        "Lagging_Current_Power_Factor",
        "Leading_Current_Power_Factor",
        "NSM",
    ]

    processor = DataProcessor()

    df[cols_to_scale] = processor.normalize_data(df[cols_to_scale])

    cols_to_drop = ["date", "Day_of_week", "WeekStatus", "Load_Type"]
    if drop_prev_load_type:
        cols_to_drop += ["_Light_Load", "_Medium_Load", "_Maximum_Load"]

    X = df.drop(cols_to_drop, axis=1)

    Y = df[["Load_Type"]]
    Y = pd.get_dummies(Y, dtype=np.int64, prefix="")

    return X, Y


def energy_task(model: MLP, sequence_length, learning_rate, batch_size, epochs, drop_prev_load_type, seed=33):
    np.random.seed(seed)

    X, Y = load_x_y(drop_prev_load_type)
    X, Y = create_sequence_data(X.values, Y.values, sequence_length=sequence_length)

    x_train, x_test, y_train, y_test = custom_train_test_split(X, Y, test_size=0.1, random_state=42)

    print("X_train shape: ", x_train.shape)
    print("Y_train shape: ", y_train.shape)

    model_file = f"{model.layers[0].__class__.__name__}_{epochs}"
    if os.path.exists(model_file):
        model = MLP.load_model(model_file)
    else:
        model.train(x_train, y_train, epochs=epochs, batch_size=batch_size, learning_rate=learning_rate)
        model.save_model(model_file)

    y_pred = model.predict(x_test)
    f1_score = model.calculate_f1_score(y_test, y_pred)
    print(f"F1 Score after test: {f1_score}")
    print(MLP.calculate_precision_recall(y_test, y_pred))
    model.print_precision_recall_table(y_test, y_pred)
    MLP.draw_roc_curve(y_test, y_pred)
