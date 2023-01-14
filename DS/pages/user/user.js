// pages/user/user.js
var app=getApp();
Page({
  getUserProfile(e) {
    // 推荐使用wx.getUserProfile获取用户信息，开发者每次通过该接口获取用户个人信息均需用户确认
    // 开发者妥善保管用户快速填写的头像昵称，避免重复弹窗
    wx.getUserProfile({
      desc: '用于完善会员资料', // 声明获取用户个人信息后的用途，后续会展示在弹窗中，请谨慎填写
      success: (res) => {
        this.setData({
          userInfo: res.userInfo,
          hasUserInfo: true
        })
        wx.setStorageSync('hasUserInfo', this.data.hasUserInfo);
        
        // wx.setStorage({key:'hasUserInfo', data:true});//判断用户登录没，存入缓存
        console.log(res);
        let nick_name = res.userInfo.nickName;
        let avatar_url = res.userInfo.avatarUrl;
        wx.setStorageSync('avatar', avatar_url);
        wx.login({
           //获取code 来与服务器交换 获取openid
          success(ret){
            console.log(ret)
            let code = ret.code;
            // wx.request({
            //   url: 'https://',
            //   method:"POST",
            //   data:{
            //     code:code,
            //     nick_name:nick_name,
            //     avatar_url:avatar_url
            //   },
            //   success(e){
            //     console.log(e)
            //     //将用户信息保存到缓存里面
            //     wx.setStorageSync('user', e.data.data)
            //   },
            //   fail(res){
            //     wx.showToast({
            //       title: '请求失败',
            //       icon:'error'
            //     })
            // }
            // })
          }
        })
      },
      fail(res){
        wx.showToast({
          title: '授权失败',
          icon:"error"
        })
      }
    })
  },
  exit:function(){
    this.setData({
      userInfo:{nickName:'登录后显示昵称',avatarUrl:'../../images/patient.png'},
      hasUserInfo: false
    })
    wx.setStorage({key:'hasUserInfo',data:this.data.hasUserInfo});
  },
  enterindex:function(){
    wx.switchTab({
      url: '../index/index',
    })
  },
  /**
   * 页面的初始数据
   */
  data: {
    userInfo: {nickName:'登录后显示昵称',avatarUrl:'../../images/patient.png'},
    hasUserInfo: false
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {

  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload() {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom() {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage() {

  }
})