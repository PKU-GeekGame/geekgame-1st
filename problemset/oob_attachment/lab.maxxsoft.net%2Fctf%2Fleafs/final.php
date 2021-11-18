<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <title>现实</title>
  <style>
    body {
      color: #eeeeee;
      background-color: #202020;
      font-family: -apple-system, BlinkMacSystemFont, Helvetica Neue, Helvetica, Arial, PingFang SC, Hiragino Sans GB, Microsoft YaHei, sans-serif;
      overflow-x: hidden;
    }
    code {
      font-family: 'Courier New', Courier, monospace;
    }
    .page {
      width: 50%;
      min-width: 300px;
      max-width: 600px;
      margin: 2em auto;
    }
    .page p {
      line-height: 1.5em;
    }
    .code-container {
      overflow-x: auto;
      padding: 1em 0;
    }
    .password {
      width: 15em;
      height: 10em;
      position: absolute;
      top: 0;
      bottom: 0;
      left: 0;
      right: 0;
      margin: auto;
      text-align: center;
    }
    .password input[type=text] {
      color: #eeeeee;
      background-color: #202020;
      border: 1px solid #eeeeee;
      border-radius: 3px;
      padding: 0.3em;
      font-family: 'Courier New', Courier, monospace;
      font-size: 1em;
    }
    .password input[type=submit] {
      color: #eeeeee;
      background-color: #202020;
      border: 1px solid #eeeeee;
      border-radius: 10em;
      padding: 0.3em;
      font-size: 1em;
      width: 6em;
      transition: all 0.1s ease-in-out;
      -webkit-transition: all 0.1s ease-in-out;
      -moz-transition: all 0.1s ease-in-out;
      -o-transition: all 0.1s ease-in-out;
    }
    .password input[type=submit]:hover {
      color: #202020;
      background-color: #eeeeee;
    }
    .password input[type=submit]:active {
      color: #202020;
      background-color: #999999;
      border-color: #999999;
    }
    .password span {
      color: #b94d4d;
      font-style: italic;
    }
  </style>
</head>
<body>
  <?php if ($_POST['password'] == 'ItsMeMrLeaf'):?>
  <div class="page">
    <p>说了那么多的回忆，不如，再来聊聊现在吧。</p>
    <p>那天和你表白之后，我们正式在一起了。回想起那段日子，整个世界都是彩色的，在校园里的每一天，我和你都很开心。</p>
    <p>但，如果你没有在8月15号出去练琴，如果你没有经过那个十字路口，如果那辆卡车没有超速。</p>
    <p>如果。</p>
    <p>如果这个世界上真的有如果。</p>
    <p>从那之后到现在，你已经昏迷整整三年了。</p>
    <p>大夫说，你失去了所有的记忆，再也想不起我们的从前，变成了一个无意识的植物人。</p>
    <p>但我依然相信，只要我每天都在床边陪着你，和你讲以前的故事，奇迹就一定会发生。</p>
    <p>如果你在那个悠长的梦里，看到了这条消息，请一定记得：</p>
    <p>从你的世界里醒过来，然后——</p>
    <p>&nbsp;</p>
    <p><b>回家吧。</b></p>
    <p>&nbsp;</p>
    <p>&nbsp;</p>
    <p class="code-container"><code>flag{W4ke_up_fr0m_7h3_L0NG_dre@m}</code></p>
  </div>
  <?php else: ?>
  <div class="password">
    <form action="." method="post">
      <p>软盘启动后的密码</p>
      <p><input type="text" name="password"></p>
      <p><input type="submit" value="继续"></p>
      <?php if (isset($_POST['password'])): ?>
      <p><span>密码错误</span></p>
      <?php endif; ?>
    </form>
  </div>
  <?php endif; ?>
</body>
</html>
