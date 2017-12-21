using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    using EffectCore;
    public sealed class Effect_WeaponTrail : Effect_WeaponTrailBase
    {
        //平滑程度,简单版本平滑程度只有40
        private int Granularity = 40;
        protected override int GetGranularity()
        {
            return Granularity;
        }
    }

}

