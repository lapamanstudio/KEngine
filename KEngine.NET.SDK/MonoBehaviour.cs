using KEngine.Components;
using System;

public class MonoBehaviour
{
    protected Transform transform;

    public MonoBehaviour() { }

    public virtual void Start() { }
    public virtual void Update(float deltaTime) { }

    public void CreateMonoBehaviour(IntPtr nativeTransformPtr)
    {
        transform = new Transform(nativeTransformPtr);
        Start();
    }
}
