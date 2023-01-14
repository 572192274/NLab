import pickle
import json
import string
def read_pk(filepath, targetpath):
    file = open(targetpath,'a',encoding='utf-8')
    jishu=0
    numlen=0
    with open(filepath,'rb') as f:
        dataset = pickle.load(f)#取dataset里的元素为列表dialog，就是一个历史对话，包含多个字典
        for dialog in dataset:
            numlen=numlen+1
            print(dialog)
            patient_word=[]
            doctor_word=[]
            num=0
            for sen in dialog:#取历史对话里面的每一个字典，就是一条话语的数据信息
                num=num+1
                sentence = sen['Sentence']
                if sen['id']=='Patient':
                    if len(doctor_word)>0:
                        #写医生数据，并置空
                        doctor = "。".join(doctor_word)
                        file.write('Doctor')
                        file.write(' ')
                        file.write(doctor)
                        file.write('\n')
                        doctor_word=[]
                    patient_word.append(sentence)
                elif sen['id']=='Doctor':
                    if len(patient_word)>0:
                        #写患者数据，并置空
                        patient = "。".join(patient_word)
                        file.write('Patient')
                        file.write(' ')
                        file.write(patient)
                        file.write('\n')
                        patient_word=[]
                    doctor_word.append(sentence)
                if num==len(dialog) and len(patient_word)>0:
                    patient = "。".join(patient_word)
                    file.write('Patient')
                    file.write(' ')
                    file.write(patient)
                    file.write('\n\n')
                if num==len(dialog) and len(doctor_word)>0:
                    doctor = "。".join(doctor_word)
                    file.write('Doctor')
                    file.write(' ')
                    file.write(doctor)
                    file.write('\n\n')
            jishu=jishu+num
            # if jishu>7000:
            #     break
            # file.write('\n')
            # if numlen==3:
            #     break
    print(jishu)
def readtext(filepath,targetpath):
    file = open(targetpath, 'a', encoding='utf-8')
    jishu = 0
    numlen = 0
    with open(filepath, 'r',encoding='utf-8') as f:
        data=f.readline()
        sentence=[]
        flag=True
        jishu=0
        while data:
            data = data.strip('\n')
            line=data.split(' ')
            if len(line)==2:
                sentence.append(line[1])
            elif len(line)==1:
                print(sentence)
                for str in sentence:
                    if len(str)>100:
                        flag=False
                        break
                if flag:
                    i=0
                    for str in sentence:
                        if(i%2==0):
                            jishu+=1
                            file.write('Patient')
                            file.write(' ')
                            file.write(str)
                            file.write('\n')
                        else:
                            jishu+=1
                            file.write('Doctor')
                            file.write(' ')
                            file.write(str)
                            file.write('\n')
                        i=i+1
                    file.write('\n')
                flag=True
                sentence=[]
            data=f.readline()
            if(jishu>6000):
                break
    print(jishu)
def read_json(filepath,targetpath):
    file = open(targetpath, 'a', encoding='utf-8')
    with open(filepath,'r',encoding='utf-8') as load_f:
        result=json.load(load_f)
    num=0
    patient_word=[]
    doctor_word=[]
    for dict in result['RECORDS']:
        print(dict)
        sentence=dict['sentence']
        sentence=sentence.replace(' ','，')
        sentence=sentence.replace('\n','。')
        sentence=sentence.replace('\r','。')
        if num!=int(dict['t']):
            num=num+1
            if len(patient_word) > 0:
                patient = "。".join(patient_word)
                file.write('Patient')
                file.write(' ')
                file.write(patient)
                file.write('\n\n')
                patient_word=[]
            if len(doctor_word) > 0:
                doctor = "。".join(doctor_word)
                file.write('Doctor')
                file.write(' ')
                file.write(doctor)
                file.write('\n\n')
                doctor_word=[]
            # if num==3:
            #     break;
        if dict['id']=='paitent':
            if len(doctor_word)>0:
                # 写医生数据，并置空
                doctor = "。".join(doctor_word)
                file.write('Doctor')
                file.write(' ')
                file.write(doctor)
                file.write('\n')
                doctor_word = []
            patient_word.append(sentence)
        elif dict['id']=='doctor':
            if len(patient_word)>0:
                # 写患者数据，并置空
                patient = "。".join(patient_word)
                file.write('Patient')
                file.write(' ')
                file.write(patient)
                file.write('\n')
                patient_word = []
            doctor_word.append(sentence)
    # 最后还有数据没写进去
    if len(patient_word) > 0:
        patient = "。".join(patient_word)
        file.write('Patient')
        file.write(' ')
        file.write(patient)
        file.write('\n\n')
        patient_word = []
    if len(doctor_word) > 0:
        doctor = "。".join(doctor_word)
        file.write('Doctor')
        file.write(' ')
        file.write(doctor)
        file.write('\n\n')
        doctor_word = []

if __name__=='__main__':

    # read_json('chunyuyisheng_xiaohuaneike.json','total_data.txt')
    # read_pk('./train.pk', './total_data.txt')
    readtext('./total_data.txt','./a.txt')