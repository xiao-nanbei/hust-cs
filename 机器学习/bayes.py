import json
from nltk.tokenize import word_tokenize

from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB
from sklearn.feature_extraction.text import TfidfTransformer
import nltk

with open("/home/databrains/dataset/train_translated.json", 'r', encoding="UTF-8") as f:
    train_data = json.load(f)
with open("/home/databrains/dataset/test_translated.json", "r", encoding="UTF-8") as f_test:
    test_data = json.load(f_test)
# nltk.download("stopwords")
# nltk.download("punkt")
WPT = nltk.WordPunctTokenizer()
stop_word_list = nltk.corpus.stopwords.words('english')
train_label_unt = []
train_sample_unt = []
test_sample_unt = []
for dic in train_data:
    train_label_unt.append(dic['label'])
    dic['content'] = dic['content'].replace('[,\.!?:()"]', '')
    dic['content'] = dic['content'].replace('[^a-zA-Z"]', ' ')
    dic['content'] = word_tokenize(dic['content'])
    temp = [word for word in dic['content'] if word not in list(stop_word_list)]
    train_sample_unt.append(' '.join(temp))
for dic in test_data:
    dic['content'] = dic['content'].replace('[,\.!?:()"]', '')
    dic['content'] = dic['content'].replace('[^a-zA-Z"]', ' ')
    dic['content'] = word_tokenize(dic['content'])
    temp = [word for word in dic['content'] if word not in list(stop_word_list)]
    test_sample_unt.append(' '.join(temp))
tfid = TfidfTransformer()
vector = CountVectorizer(max_features=4000)
clf = MultinomialNB(alpha=0.0100, fit_prior=False, class_prior=[0.4, 0.6])
X_train_vector = vector.fit_transform(train_sample_unt)
X_test_vector = vector.transform(test_sample_unt)
X_train = tfid.fit_transform(X_train_vector)
X_test = tfid.transform(X_test_vector)
clf.fit(X_train, train_label_unt)
test_label = clf.predict(X_test)
for cons_label in test_label:
    with open("/home/databrains/dataset/cons_label.txt", "a", encoding='utf-8') as cons_file:
        cons_file.write(str(cons_label))
        cons_file.write('\n')
