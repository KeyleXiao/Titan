using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace DataCenter
{
    class CreateHeroDataManager :Singleton<CreateHeroDataManager>, IModelData
    {
        /// <summary>
        /// 创建英雄时玩家名字
        /// </summary>
        public string PlayerName = null;
        /// <summary>
        /// 创建英雄时玩家性别
        /// </summary>
        public PERSON_SEX PlayerSex = PERSON_SEX.SEX_MAX;
        /// <summary>
        /// 创建英雄时玩家选择的英雄ID
        /// </summary>
        public int PlayerCreateHeroID = -1;
        public void Init()
        {
            
        }
    }
}
