/*
 * Copyright 2013 ThirdMotion, Inc.
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *		Unless required by applicable law or agreed to in writing, software
 *		distributed under the License is distributed on an "AS IS" BASIS,
 *		WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *		See the License for the specific language governing permissions and
 *		limitations under the License.
 */

/**
 * @class strange.extensions.mediation.impl.MediationException
 * 
 * An exception thrown by the Mediation system
 */

using System;
using strange.extensions.mediation.api;

namespace strange.extensions.mediation.impl
{
	public class MediationException : Exception
	{
		public MediationExceptionType type{ get; set;}

		public MediationException() : base()
		{
		}

		/// Constructs a MediationException with a message and MediationExceptionType
		public MediationException(string message, MediationExceptionType exceptionType) : base(message)
		{
			type = exceptionType;
		}
	}
}

