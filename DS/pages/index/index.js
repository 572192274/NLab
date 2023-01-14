// index.js
// 获取应用实例
const app = getApp()

Page({
  data: {
    hasUserInfo:false
  },
  //医疗咨询 胃肠
  wc_repo:function(){
    this.generate();
    if(this.data.hasUserInfo){
      wx.navigateTo({
        url: '../wc_medical/wc_medical',
      })
    }
  },
  //医疗咨询
  medical_repo:function(){
      this.generate();
      if(this.data.hasUserInfo){
        wx.navigateTo({
          url: '../medical/medical',
        })
      }
  },
  //情感咨询闲聊
  emo_repo:function(){
    this.generate();
    if(this.data.hasUserInfo){
      wx.navigateTo({
        url: '../emo/emo',
      })
    }
  },
  //情感咨询，情侣
  zfqlg_repo:function(){
    this.generate();
    if(this.data.hasUserInfo){
      wx.showModal({
        title:'提醒',
        content:'待开发',
        success(res){
          if(res.confirm){
            wx.switchTab({
              url: '../zfqlg_emo/zfqlg_emo',
            })
          }
        }
      })
    }
  },
  //判断用户是否登录
  generate:function(){
    var value=wx.getStorageSync('hasUserInfo');
    this.setData({
      hasUserInfo:value
    })
    if (!value) {
      wx.showModal({
        title:'提醒',
        content:'登录后解锁完整功能',
        confirmText: '去登录',
        success(res){
          if(res.confirm){
            wx.switchTab({
              url: '../user/user',
            })
          }
        }
      })
    }
  }
})
