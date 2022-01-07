import numpy as np  # linear algebra
import pandas as pd  # data processing, CSV file I/O (e.g. pd.read_csv)
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
from tensorflow.python.keras.preprocessing.text import Tokenizer
from sklearn.model_selection import train_test_split
from tensorflow.python.keras.models import Sequential
from tensorflow.python.keras.layers import Dense, Embedding, LSTM
from tensorflow.python.keras.preprocessing.text import Tokenizer
from keras.optimizer_v2.adam import Adam
from keras.layers import Dropout
from keras.models import Sequential
from tensorflow.python.keras.preprocessing.sequence import pad_sequences
from tensorflow.python.keras.models import load_model
from sklearn.model_selection import train_test_split
import re
import nltk
from nltk.corpus import stopwords
import warnings
import os
import keras

# nltk.download("stopwords")
# nltk.download("punkt")
# nltk.download('punkt')
warnings.filterwarnings("ignore")
data = pd.read_csv("/home/databrains/train.csv")
WPT = nltk.WordPunctTokenizer()
stop_word_list = nltk.corpus.stopwords.words('english')
data['content'] = data['content'].apply(lambda x: re.sub('[,\.!?:()"]', '', x))
data['content'] = data['content'].apply(lambda x: re.sub('[^a-zA-Z"]', ' ', x))
data['content'] = data['content'].apply(lambda x: x.lower())
data['content'] = data['content'].apply(lambda x: x.strip())
sentiment = data['label'].values
data = data['content']
x_train, x_test, y_train, y_test = train_test_split(data, sentiment, test_size=0.2, random_state=42)
tokenizer = Tokenizer(num_words=15000)
tokenizer.fit_on_texts(data)
x_train_tokens = tokenizer.texts_to_sequences(x_train)
x_test_tokens = tokenizer.texts_to_sequences(x_test)
num_tokens = [len(tokens) for tokens in x_train_tokens + x_test_tokens]
num_tokens = np.array(num_tokens)
max_tokens = np.mean(num_tokens) + 2 * np.std(num_tokens)
max_tokens = int(max_tokens)
x_train_pad = pad_sequences(x_train_tokens, maxlen=max_tokens)
x_test_pad = pad_sequences(x_test_tokens, maxlen=max_tokens)
idx = tokenizer.word_index
inverse_map = dict(zip(idx.values(), idx.keys()))


def return_to_sentence(tokens):
    words = [inverse_map[token] for token in tokens if token != 0]
    text = ' '.join(words)
    return text


model = Sequential()
embedding_size = 50
model.add(Embedding(input_dim=15000, output_dim=embedding_size, input_length=max_tokens, name='embedding_layer'))
model.add(LSTM(units=8, return_sequences=True))
model.add(Dropout(0.1))
model.add(LSTM(units=4))
model.add(Dropout(0.1))
model.add(Dense(1, activation='hard_sigmoid'))
optimizer = Adam(lr=1e-3)
model.compile(loss='binary_crossentropy', optimizer=optimizer, metrics=['accuracy'])
history = model.fit(x_train_pad, y_train, validation_split=0.3, epochs=5, batch_size=1000, shuffle=True, verbose=1)
# result = model.evaluate(x_test_pad, y_test)

data1 = pd.read_csv("/home/databrains/test.csv")
WPT = nltk.WordPunctTokenizer()
stop_word_list = nltk.corpus.stopwords.words('english')
data1['content'] = data1['content'].apply(lambda x: re.sub('[,\.!?:()"]', '', x))
data1['content'] = data1['content'].apply(lambda x: re.sub('[^a-zA-Z"]', ' ', x))
data1['content'] = data1['content'].apply(lambda x: x.lower())
data1['content'] = data1['content'].apply(lambda x: x.strip())
data1 = data1['content']
tokenizer = Tokenizer(num_words=15000)

tokenizer.fit_on_texts(data1)
x_tokens1 = tokenizer.texts_to_sequences(data1)
num_tokens1 = [len(tokens) for tokens in x_tokens1]
num_tokens1 = np.array(num_tokens1)
max_tokens1 = np.mean(num_tokens1) + 2 * np.std(num_tokens1)
max_tokens1 = int(max_tokens1)
x_pad1 = pad_sequences(x_tokens1, maxlen=max_tokens1)
idx1 = tokenizer.word_index
inverse_map1 = dict(zip(idx1.values(), idx1.keys()))

result = model.predict(x_pad1, batch_size=16, verbose=0, steps=None)
result_f = [None] * 907
sorted_id = sorted(range(len(result)), key=lambda k: result[k], reverse=True)
fp = open("/home/databrains/result.txt", 'w+')
for index in range(len(sorted_id)):
    if index < 825:
        result_f[sorted_id[index]] = 1
    else:
        result_f[sorted_id[index]] = 0
for index in range(len(result_f)):
    fp.write(str(result_f[index]) + '\n')
fp.close()
print(result)
