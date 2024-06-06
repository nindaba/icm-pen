from tensorflow.keras.models import load_model
import numpy as np
import pandas as pd
from sklearn.preprocessing import LabelEncoder
import glob
import os

import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.metrics import precision_score, \
    recall_score, f1_score, accuracy_score


def load_data(base_paths='data/letters_test', data_paths=[]):
    data = []
    labels = []
    for path in data_paths:
        df = pd.read_csv(os.path.join(base_paths, path))
        letter = path.split('/')[2]
        data.append(df)
        labels.append(letter)
    data = np.array(data)
    return data, labels


def predict_letters():
    model = load_model('ml/bin/icm_letters_v2.maxe.keras')
    le = LabelEncoder()
    le.classes_ = np.load('ml/bin/label_encoder.npy')
    test_dir = "data/letters_grouped_test/*/*"
    data_paths = glob.glob(test_dir)
    data, labels = load_data(base_paths="", data_paths=data_paths)
    evaluate = model.predict(data)
    predicted = [le.inverse_transform([np.argmax(i)])[0] for i in evaluate]
    print("Accuracy: ", accuracy_score(labels, predicted))

    confusion_matrix = pd.crosstab(pd.Series(labels), pd.Series(predicted), rownames=['Actual'], colnames=['Predicted'])
    sns.heatmap(confusion_matrix, annot=True)
    plt.show()


predict_letters()
