using UnityEngine;

namespace Effect
{
    namespace TrailsSystem
    {
        public class Effect_SmokeTrail : Effect_TrailRenderer_Base
        {
            public float MinVertexDistance = 0.1f;
            public int MaxNumberOfPoints = 50;
            private Vector3 _lastPosition;
            private float _distanceMoved;
            public float RandomForceScale = 1;

            protected override void OnStart()
            {
                base.OnStart();
                _lastPosition = _t.position;

            }


            protected override void AfterUpdate()
            {
                if (_emit)
                {
                    _distanceMoved += Vector3.Distance(_t.position, _lastPosition);

                    if (_distanceMoved != 0 && _distanceMoved >= MinVertexDistance)
                    {
                        AddPoint(new SmokeTrailPoint(), _t.position);
                        _distanceMoved = 0;
                    }
                    _lastPosition = _t.position;

                }

                base.AfterUpdate();
            }

            protected override void OnStartEmit()
            {
                base.OnStartEmit();
                _lastPosition = _t.position;
                _distanceMoved = 0;
            }

            public override void Reset()
            {
                base.Reset();
                MinVertexDistance = 0.1f;
                RandomForceScale = 1;
            }

            protected override void InitialiseNewPoint(PCTrailPoint newPoint)
            {
                ((SmokeTrailPoint)newPoint).RandomVec = Random.onUnitSphere * RandomForceScale;
            }
            protected override void OnTranslate(Vector3 t)
            {
                _lastPosition += t;
            }

            protected override int GetMaxNumberOfPoints()
            {
                return MaxNumberOfPoints;
            }
        }

        public class SmokeTrailPoint : PCTrailPoint
        {
            public Vector3 RandomVec;

            public override void Update(float deltaTime)
            {
                base.Update(deltaTime);
                Position += RandomVec * deltaTime;
            }
        }
    }
}


