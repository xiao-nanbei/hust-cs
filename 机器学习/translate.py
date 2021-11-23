import json
import textwrap
from google_trans_new import google_translator


def read_data(path, is_train, output_path):
    with open(path, 'r') as f:
        data = json.load(f)
    content = []
    res_label = []
    if is_train:
        for each_content in data:
            content.append(each_content["content"].lower())
            res_label.append(each_content["label"])
        res_content = [None] * len(content)
        for index in range(0, 300):
            print(index)
            if len(content[index]) >= 5000:
                str = textwrap.wrap(content[index], 2000)
                for i in range(len(str)):
                    t = google_translator(timeout=10)
                    str[i] = t.translate(text=str[i], lang_tgt='en')
                temp = '\n'
                res_content[index] = temp.join(str)
            else:
                t = google_translator(timeout=10)
                res_content[index] = t.translate(text=content[index], lang_tgt='en')
            print(res_content[index])
        translated_json = []
        for i in range(600, 906):
            _dict = {}
            _dict["content"] = res_content[i]
            _dict["label"] = res_label[i]
            translated_json.append(_dict)
        with open(output_path, 'w') as f:
            json.dump(translated_json, f)
    else:
        for each_content in data:
            content.append(each_content["content"].lower())
        res_content = [None] * len(content)
        for index in range(600, 907):
            print(index)
            if len(content[index]) >= 5000:
                str = textwrap.wrap(content[index], 2000)
                for i in range(len(str)):
                    t = google_translator(timeout=10)
                    str[i] = t.translate(text=str[i], lang_tgt='en')
                temp = '\n'
                res_content[index] = temp.join(str)
            else:
                t = google_translator(timeout=10)
                res_content[index] = t.translate(text=content[index], lang_tgt='en')
            print(res_content[index])
        translated_json = []
        for i in range(600, 907):
            _dict = {}
            _dict["content"] = res_content[i]
            translated_json.append(_dict)
        with open(output_path, 'w') as f:
            json.dump(translated_json, f)
    return res_content, res_label


if __name__ == "__main__":
    '''
    #这里的代码是调用google_tranlate进行翻译，只能分批次翻译，不然会被google_translate ban
    path = "/home/databrains/dataset/test.json"
    output_path = "/home/databrains/dataset/test_translated_3.json"
    read_data(path, False, output_path)
    '''
    #这里的代码是组合翻译好的文件
    temp = []
    for index in range(1, 4):
        with open("/home/databrains/dataset/test_translated_" + str(index)+".json", 'r') as f:
            data = json.load(f)
        for each_content in data:
            temp.append(each_content)
    with open("/home/databrains/dataset/test_translated.json", 'w') as f:
        json.dump(temp, f)
