﻿using UnityEngine;

namespace Effect
{
    namespace TrailsSystem
    {
        public class Effect_SmokePlume : Effect_TrailRenderer_Base
        {
            public float TimeBetweenPoints = 0.1f;
            public Vector3 ConstantForce = Vector3.up * 0.5f;
            public float RandomForceScale = 0.05f;
            public int MaxNumberOfPoints = 50;
            private float _timeSincePoint;

            protected override void OnStart()
            {
                base.OnStart();
                _timeSincePoint = 0;
            }

            protected override void OnStartEmit()
            {
                base.OnStartEmit();
                _timeSincePoint = 0;
            }

            public override void Reset()
            {
                base.Reset();
                TrailData.SizeOverLife = new AnimationCurve(new Keyframe(0, 0), new Keyframe(0.5f, 0.2f), new Keyframe(1, 0.2f));
                TrailData.Lifetime = 6f;
                ConstantForce = Vector3.up * 0.5f;
                TimeBetweenPoints = 0.1f;
                RandomForceScale = 0.05f;
                MaxNumberOfPoints = 50;
            }

            protected override void AfterUpdate()
            {
                if (_emit)
                {
                    _timeSincePoint += _noDecay ? 0 : Time.deltaTime;

                    if (_timeSincePoint >= TimeBetweenPoints)
                    {
                        AddPoint(new SmokeTrailPoint(), _t.position);
                        _timeSincePoint = 0;
                    }
                }
                base.AfterUpdate();
            }

            protected override void InitialiseNewPoint(PCTrailPoint newPoint)
            {
                ((SmokeTrailPoint)newPoint).RandomVec = Random.onUnitSphere * RandomForceScale;
            }

            protected override void UpdateTrail(PCTrail trail, float deltaTime)
            {
                if (_noDecay)
                    return;

                foreach (PCTrailPoint point in trail.Points)
                {
                    point.Position += ConstantForce * deltaTime;
                }
            }
            protected override int GetMaxNumberOfPoints()
            {
                return MaxNumberOfPoints;
            }
        }


    }
}


