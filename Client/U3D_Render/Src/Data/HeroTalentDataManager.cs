using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI.HeroTalentPart;

namespace DataCenter
{
    class HeroTalentDataManager :Singleton<CreateHeroDataManager>, IModelData
    {
        private Dictionary<int, List<HeroTalentInfo>> dicTalentInfoData = new Dictionary<int, List<HeroTalentInfo>>();
        private int[] LevelArray;
        private int m_nCurMaxLevel = 0;
        private int m_nCurHeroId = 0;
        public void Init()
        {
            
        }
    }
}
