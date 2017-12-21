using ASpeedGame.Data.GameMobaSchemes;
using UnityEngine;

namespace USpeedUI.UWidgets
{
    public class UNPCGoodsItem
    {
        // NPC实体ID
        public int uidNPC;
        // 冷却时间
        public int nCoolTime;
        // 最近一次购买时间
        public int nLastBuyTime;
        public SSchemeMobaGoods schemeGoods;
    }
}
