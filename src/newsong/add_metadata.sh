#!/bin/bash

if [ "$#" -ne 3 ]; then
  echo "usage: $0 INPUT_MP3 COVER OUTPUT_MP3" >&2
  exit 1
fi

ffmpeg -i $1 -i $2 -map 0:0 -map 1:0 -c copy -id3v2_version 3 -metadata:s:v comment="Cover (front)" \
  -metadata title="叶子的新歌" -metadata artist="叶子" \
  -metadata album="Secret in Album Cover!!" -metadata TRACKTOTAL="aHR0cDovL2xhYi5tYXh4c29mdC5uZXQvY3RmL2xlZ2FjeS50Ynoy" \
  -metadata lyrics="空无一人的房间
我望向窗外
想回到昨天

琥珀色的风
能否将 回忆传到那边
闪烁的星
照亮夜空 连成我的思念

你 在梦的另一边
站在 日落的地平线
背离这世界而去
想 在回不去的时间里
遇见你 遇见你 遇见你
遇见你 遇见你 遇见你" \
  -metadata comment="你还记得吗？小时候，我家和你家都在一个大院里。放学以后，我们经常一起在院子里玩。你虽然是个女孩子，但总是能和男孩子们玩到一块去。

夏天的时候我们挖蚯蚓、捉蚂蚱；冬天，院子里的大坡上积了一层雪，我们就坐在纸箱子压成的雪橇上，一次次从坡顶滑到坡底。那个时候你还发现，坐在铁簸箕上滑得更快。

——当然，那次你也摔得挺惨的。" $3
