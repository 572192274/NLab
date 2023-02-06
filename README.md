# NLab

## Labeling specification

The dataset should be extensible and able to adapt to the application scenario of multiple tasks. The dataset is marked with fine granularity. For a round of dialogue, the intention, slot-value and entity scope of each sentence of patients and doctors are labeled. Intents aim to determine the speaker's motivation, which helps analyze the semantics of generated statements. Patient intents and doctor intents are specified by role, and common intents include inquire and chitchat. Slots are designed to give each sentence a domain, equivalent to an entity meaning, and specify several slots for conversation generation tasks. Slot-value and entity ranges are specific tagging information for each sentence and can be used for tasks such as named entity recognition, disease, and symptom prediction. Patient and doctor intention are shown in Tables 1 and 2.

- Table 1 Definition of patient intent

| **Intention** | **Role** | **Explain**                            | **Example**                                                  |
| ------------- | -------- | -------------------------------------- | ------------------------------------------------------------ |
| Inform        | Patient  | The  patient informs the doctor        | da bian zheng chang,ye bu e  xin  大便正常，也不恶心         |
| Inquire       | Patient  | The  patient asks the doctor           | wo de le ji xing fu xie,gai  ru he zhi liao?  我得了急性腹泻，该如何治疗？ |
| Want  confirm | Patient  | Want  to get the doctor's confirmation | shi chang dao xi jun bu ping  heng ma?  是肠道细菌不平衡吗？ |
| Confirm       | Patient  | Reply  to the doctor for confirmation  | you  有                                                      |
| Deny          | Patient  | Deny  the doctor's confirmation        | fa shao mei you  发烧没有                                    |
| Chitchat      | Patient  | Chitchat  between patient and doctor   | hao de, xie xie  好的，谢谢                                  |

-  Table 2 Definition of doctor intent

| **Intention**       | **Role** | **Explain**                                      | **Example**                                             |
| ------------------- | -------- | ------------------------------------------------ | ------------------------------------------------------- |
| Diagnosis           | Doctor   | Doctor  diagnose about patient                   | zhe shi shi guan fan liu  这是食管反流                  |
| Recommendation      | Doctor   | Advice  given by a doctor to a patient           | jian yi chi dian ao mei la  zuo  建议吃点奥美拉唑       |
| Not  recommendation | Doctor   | Things  that doctors don't advise patients to do | bi mian shi yong ci ji xing  shi wu  避免使用刺激性食物 |
| Want  confirm       | Doctor   | Want  confirmation                               | bian mi you me  便秘有吗                                |
| Confirm             | Doctor   | Reply  to the patient for confirmation           | shi de  是的                                            |
| Deny                | Doctor   | Deny  the patient's confirmation                 | bu ke  yi  不可以                                       |
| Inquire             | Doctor   | The  doctor questioned the patient               | hai you qi ta zheng Zhuang  ma？  还有其他症状吗？      |
| Chitchat            | Doctor   | Chitchat  between doctor and patient             | hao de, xie xie  好的，谢谢                             |

Slots can be regarded as a category, such as diseases, symptoms, and drugs. Slots are the specific manifestations of slots. Each slot may have multiple slots. The specific slot definition is shown in Table 3, and the slot value can only be obtained by specific analysis of a sentence.

- Table 3 Slot definition

| **Slot**                | **Slot  value**                                              | **Explain**                                                  | **Example**                                                  |
| ----------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Symptom                 | Multiple  values can be identified from statements or referenced to medical standard  terms | Statement for symptoms that may  occur                       | du qi zhou wei  【yin yin zuo tong】  肚脐周围【隐隐作痛】   |
| Disease                 | Multiple  values can be identified from statements or referenced to medical standard  terms | Statements  that refer to diseases that may not appear explicitly | shu yu 【gao zhi xing yi xian yan】  属于【高脂性胰腺炎】。  |
| Drug                    | Identified from the statement, there can be  multiple values | The name of the drug mentioned in the statement              | ci shu duo jiu chi 【si mi da】  次数多就吃【思密达】        |
| Check                   | Identified from the statement, there can be  multiple values | The statement mentions the check that was done               | You cha  【xue chang  gui】ma  有查【血常规】吗              |
| Symptomatic site        | Identified from the statement, there can be  multiple values | Statement where symptoms occur                               | 【zuo ce lei  gu bu wei】hen teng  【左侧肋骨部位】很疼      |
| Food name               | Identified from the statement, there can be  multiple values | The food mentioned in the statement                          | man xing chang wei yan hui fu qi ke yi chi【zhi shi】ma  慢性肠胃炎恢复期可以吃【芝士】吗 |
| Treatment site          | Identify from the statement                                  | The treatment site mentioned in the statement                | 【san jia yi  yuan】wu tong wei jing yi ban qi bai duo  【三甲医院】无痛胃镜一般七百多 |
| Department              | Identify from the statement                                  | The department mentioned in the statement                    | dao【pi fu ke】kan yi xia  到【皮肤科】看一下                |
| Body part               | Identify from the statement                                  | The part of the body mentioned in the statement              | bu hui qian che dao 【zuo bian jian bu】  不会牵扯到【左边肩部】 |
| Dosage of drug          | Identify from the statement                                  | The dosage recommended by the doctor                         | pi wei xiu an pian【yi ci yi pian】  匹维溴铵片【一次一片】  |
| Frequency of medication | Identify from the statement                                  | The frequency of medication recommended by your  doctor      | pei fei kang jiao nang,【yi tian liang  ci】  培菲康胶囊，【一天两次】 |
| Preventive measure      | Identify from the statement                                  | The advice provided in the statement to  alleviate the condition | 【shao chi  dian rou zhi pin, he dou zhi pin】, 【duo chi shu cai shui guo】  【少吃点肉制品，和豆制品】，【多吃蔬菜水果】 |
| Drug class              | Identify from the statement                                  | Class of drug mentioned in the statement                     | 【a mo xi lin】shi xiao yan yao  【阿莫西林】是消炎药        |
| State                   | Positive, negative, other                                    | Whether the proposed disease or symptom is real              | -                                                            |
| Time                    | Identify from the statement                                  | The time and duration of symptoms described by  the patient  | 【liang san  tian】 le.  【两三天】了。                      |
| Behavior                | Identify from the statement                                  | An expression that refers to an action in a  statement       | yin yin zuo tong  隐隐作痛  shi liang shao chi  适量少吃     |

##  Medical dialogue system

The Medical Dialogue system is in the DS directory. Use wechat mini program to build the client, and use python to complete the dialogue process at the back end. The medical dialogue system is mainly composed of three modules: natural language understanding module: obtaining the intention expressed in the patient text and extracting the corresponding medical entity; Knowledge fusion module: fusion of the current questions, predicted entities, dialogue history text and entity-relationship, role information; Dialogue generation module: converts the fused information into understandable text information.

## Environment setting

QT4.3.1

python3.7

Wechat developer tools

