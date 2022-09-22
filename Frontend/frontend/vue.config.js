module.exports = {
  chainWebpack: config => {
    config.module
      .rule('raw')
      .test(/\.xml$/)
      .use('raw-loader')
      .loader('raw-loader')
      .end()
  }
}
