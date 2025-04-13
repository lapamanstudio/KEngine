using Kengine.Components;
using System;
using System.Runtime.CompilerServices;

namespace KEngine.Components
{
    public class Transform
    {
        private IntPtr nativeTransformPtr;

        public Transform(IntPtr nativeTransformPtr)
        {
            this.nativeTransformPtr = nativeTransformPtr;
        }

        public Vector2 Position
        {
            get
            {
                Vector2 position;
                Internal_GetPosition(nativeTransformPtr, out position);
                return position;
            }
            set
            {
                Internal_SetPosition(nativeTransformPtr, ref value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPosition(IntPtr nativeTransformPtr, out Vector2 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPosition(IntPtr nativeTransformPtr, ref Vector2 position);
    }
}
