// pages/medical/medical.js
const app = getApp();
let inputVal = '';  //输入框的内容，未发
let msgList = [];   //聊天记录，已发
let windowWidth = wx.getSystemInfoSync().windowWidth;  //可使用屏幕宽度
let windowHeight = wx.getSystemInfoSync().windowHeight;//可使用屏幕高度
let keyHeight = 0; //键盘弹起高度
 
/**
 * 初始化数据
 */
function initData(that) {
  inputVal = '';
 //speaker：server，customer
  msgList = [
    {
      speaker: 'server',
      contentType: 'text',
      content: '你好'
    }
  ]
  var value=wx.getStorageSync('avatar');
  that.setData({
    msgList,
    inputVal,
    picHead:value
  })
  wx.connectSocket({
    url: 'wss://192.168.1.8:5555/dialog/emo',
    success:function(res){
      console.log("成功")
      
    },
    fail:function(res){
      console.log("失败")
      
    }
  })
  wx.onSocketOpen((result) => {
    console.log("成功连接")
    that.data.socketOpen=true
  })
  //向服务器接收数据
  wx.onSocketMessage((result) => {
    // var data=JSON.parse(result.data)
    var data=result.data
    msgList.push({
      speaker: 'server',
      contentType: 'text',
      content:data
    })
    console.log(msgList)
    inputVal=''
    that.setData({
      msgList,
      inputVal
    })
    
    
  })
  //监听连接关闭
  wx.onSocketClose((result) => {
    console.log('服务器关闭')
    msgList.push({
      speaker: 'server',
      contentType: 'text',
      content:'未在线'
    })
    that.setData({
      msgList
    })
  })
  wx.onSocketError((result) => {
    msgList.push({
      speaker: 'server',
      contentType: 'text',
      content:'未在线'
    })
    that.setData({
      msgList
    })
  })
}

Page({
  
/**
   * 页面的初始数据
   */
  data: {
    scrollHeight: '100vh', //这个高度一定要有
    inputBottom: 0,
    inputVal: '',
    picHead: '../../images/girl.png',
    socketOpen:false
  },
 
  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    initData(this);
    
    // this.setData({
    //   picHead: app.globalData.userInfo.picHead,
    // });
  },
 
  // 获取聚焦,键盘弹起
  focus: function(e) {
    keyHeight = e.detail.height;
    this.setData({
      scrollHeight: (windowHeight - keyHeight) + 'px'
    });
    this.setData({
      toView: 'msg-' + (msgList.length - 1),
      inputBottom: keyHeight + 'px'
    })
  },
 
  //失去聚焦(软键盘消失)
  blur: function(e) {
    this.setData({
      scrollHeight: '100vh',
      inputBottom: 0
    })
    this.setData({
      toView: 'msg-' + (msgList.length - 1)
    })
 
  },
 
  // 发送点击监听
  sendClick: function(e) {
    msgList.push({
      speaker: 'customer',
      contentType: 'text',
      content: e.detail.value
    })
    wx.sendSocketMessage({
      data: e.detail.value,
    })
    inputVal = '';
    this.setData({
      msgList,
      inputVal
    });
  },
 
  // nav栏返回
  onClickLeft() {
    wx.reLaunch({
      url: '/pages/message/message',
    })
  },
  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {
    
  },
 
  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
 
  },
 
  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {
 
  },
 
  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
    wx.sendSocketMessage({
      data: 'history',
    })
    wx.closeSocket({
      code: 0,
      success(){
        console.log('关闭连接')
      }
    })
    

  },
 
  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {
 
  },
 
  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {
 
  },
 
  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
 
  }

})