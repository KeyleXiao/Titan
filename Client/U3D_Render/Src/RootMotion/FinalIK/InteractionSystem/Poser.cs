using UnityEngine;
using System.Collections;

namespace RootMotion.FinalIK {

	/// <summary>
	/// The base abstract class for all class that are translating a hierarchy of bones to match the translation of bones in another hierarchy.
	/// </summary>
	public abstract class Poser: MonoBehaviour {

		/// <summary>
		/// Reference to the other Transform (should be identical to this one)
		/// </summary>
		public Transform poseRoot;
		/// <summary>
		/// The master weight.
		/// </summary>
		[Range(0f, 1f)] public float weight = 1f;
		/// <summary>
		/// Weight of localRotation matching
		/// </summary>
		[Range(0f, 1f)] public float localRotationWeight = 1f;
		/// <summary>
		/// Weight of localPosition matching
		/// </summary>
		[Range(0f, 1f)] public float localPositionWeight;

		/// <summary>
		/// Map this instance to the poseRoot.
		/// </summary>
		public abstract void AutoMapping();

	}
}
