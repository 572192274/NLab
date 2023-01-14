# ANTL

## 1、导入功能

功能描述：将原始格式的数据导入到数据库存储，用于之后标注员从数据库中获取数据进行标注

### （1）原始数据格式

来源为春雨医生的消化内科医患对话，包含旧数据[https://github.com/lwgkzl/MedDG]和刚爬取的新数据

格式如下：

![image-20220228215104115](https://gitee.com/liugang19970527/antl/raw/master/%E6%A0%87%E6%B3%A8%E8%A7%84%E8%8C%83/img/1.png)

### （2）数据库表

**表1 utterances**

保存所有对话信息的数据表，由管理员手动一次性导入。

| 列名           | 类型                  | 说明                                   |
| -------------- | --------------------- | -------------------------------------- |
| utteranceId    | int unsigned not null | 主键，对话字段编号                     |
| conversationId | int unsigned not null | 对话列表编号                           |
| utterance      | text                  | 对话正文文本                           |
| sequenceNo     | int unsigned not null | 对话在对话列表中的顺序号               |
| agentRole      | varchar(100) not null | 该条对话属于的角色，枚举值：医生\|用户 |
| annotation     | text                  | 对话标注，json格式                     |

**表2 conversations**

保存对话列表额外信息的数据表，导入表1时同步生成，再由用户添加额外信息

| 列名              | 类型                  | 说明                           |
| ----------------- | --------------------- | ------------------------------ |
| Id                | int unsigned not null | 主键，自增长                   |
| conversationId    | int unsigned not null | 对话列表编号                   |
| conversationState | int unsigned not null | 对话列表状态                   |
| userName          | varchar(100) not null | 处理该对话列表的用户           |
| agentId           | int unsigned          | 该条对话属于的角色id，可以为空 |

conversationState有3种状态：

1. "0"代表该段对话无人进行过处理，当用户点击**获取对话进行标注**时，系统会随机提供一段处于此状态的对话给用户，并将该段对话状态置为"1"。
2. "1"代表该段对话已经分配给某一个用户，但是未完成标注。用户打开系统时，系统判断该用户的历史标注对话是否有状态为"1"的对话，如有则打开该段对话；没有，则提供状态为"0"的对话给该用户，并将该段对话状态置为"1"。
3. "2"代表用户完成标注，是否完成标注通过用户是否点击“完成标注”这按钮判断。

**表3 user**

存储用户信息的表

| 列名     | 类型                  | 说明                                        |
| -------- | --------------------- | ------------------------------------------- |
| userId   | int unsigned not null | 主键，用户的id                              |
| userName | varchar(100) not null | 用户名，默认为userId                        |
| password | varchar(100) not null | 用户密码                                    |
| classes  | int unsigned not null | 用户类别（2：管理员，1：审核员，0：标注员） |

## 2、标注功能

功能描述：用户（标注员）从数据库中获取完整的医患对话数据进行展示，点击对话，可以对其进行意图、意图范围、槽值、槽值范围进行标注，并能显示出一个字在整个句子中的位置，方便判断意图和槽值范围。

### （1）标注数据的格式存储

```json
[
    {
    "intent":"inform",
    "slot_values":[
           {"slot":"", "value":"", "start":0, "end":1},
           {"slot":"", "value":"", "start":0, "end":2},
           ...
      ]
    
    },
    
    {
    "intent":"inform",
    "slot_values":[
           {"slot":"", "value":"", "start":0, "end":1},
           {"slot":"", "value":"", "start":0, "end":1},
           ...
      ]
    
    },
    ...
]
```



## 3、导出功能

功能描述：通过读取数据库中已标注的数据，按照设计的导出格式进行导出，用于模型训练方便读取训练数据

### （1）导出格式

```
[
    {
        "conversationId": "1001",
        "utterances":[
            {"agentRole":"Patient",
              "utterance":"我肚子疼",
              "sequenceNo":"1", 
              "annotation":[]   
            },
             {"agentRole":"Doctor",
              "utterance":"……",
              "sequenceNo":"2",
              "annotation":[]  /*最新版本的*/
            },
           ……
            ]
            
        
    },
    {
        "conversationId": "1002", 
        "utterances":[
            {"agentRole":"Patient",
              "utterance":"……",
              "sequenceNo":"1", 
              "annotation":[]   /*最新版本的*/
            },
             {"agentRole":"Doctor",
              "utterance":"……",
              "sequenceNo":"2",
              "annotation":[]  /*最新版本的*/
            },
           ……
            ]
            
        
    },
    ……
]
```



## 4、审核功能

功能描述：随机抽取数据库中已标注的数据，审核标注的数据是否正确

## 5、界面设计

![image-20220302184828972](https://gitee.com/liugang19970527/antl/raw/master/%E6%A0%87%E6%B3%A8%E8%A7%84%E8%8C%83/img/2.png)

### 登录界面

![image-20220314005344785](https://gitee.com/liugang19970527/antl/raw/master/%E6%A0%87%E6%B3%A8%E8%A7%84%E8%8C%83/img/3.png)

### 管理员界面

![image-20220314005445553](https://gitee.com/liugang19970527/antl/raw/master/%E6%A0%87%E6%B3%A8%E8%A7%84%E8%8C%83/img/4.png)

### 审核员界面

![image-20220314005551649](https://gitee.com/liugang19970527/antl/raw/master/%E6%A0%87%E6%B3%A8%E8%A7%84%E8%8C%83/img/5.png)

### 标注员界面

![image-20220314005623749](https://gitee.com/liugang19970527/antl/raw/master/%E6%A0%87%E6%B3%A8%E8%A7%84%E8%8C%83/img/6.png)

描述：

1、点击导入数据执行导入功能

2、点击审核数据执行审核功能

3、点击导出功能执行导出功能

4、点击标注数据执行标注功能，左边显示一次医患对话数据，当点击一个句子时，右边显示标注信息，点击加号，增加一排标注，确认标注正确后，点击提交，更新数据表