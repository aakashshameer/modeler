#include "customprop.h"

#include <scene/sceneobject.h>

REGISTER_COMPONENT(CustomProp, CustomProp)

CustomProp::CustomProp()
    : mp_root(NULL)
    , m_angle(0.0f, 0.0f, 359.0f, 1.0f) //for slider control (default value, min value, max value, step value). Remove this you don't want to the slider control
    , m_RAF(1.0f, 1.0f, 100.0f, 1.0f)
    //, m_your_prop(0.0f, 0.0f, 359.0f, 1.0f)
{
    AddProperty("Angle", &m_angle);
    m_angle.ValueChanged.Connect(this, &CustomProp::OnAngleChanged);

    /*
    AddProperty("Your Prop", &m_your_prop);
    m_your_prop.ValueChanged.Connect(this, &CustomProp::OnYourPropChanged);
    */
    AddProperty("Ready, Aim, Fire", &m_RAF);
    m_RAF.ValueChanged.Connect(this, &CustomProp::OnRAFChanged);
}

void CustomProp::OnAngleChanged(double angle)
{
    if (mp_root == NULL)
        return;

    std::cout << "angle = " << angle << std::endl;

    // REQUIREMENT: Modify the code snippet below to realize your customized control for your created model
    //              You could also refer to RobotArmProp.cpp to know how it works
    ////////////////////////////////////////////////////////////////////
    // Find the child node whose name is "PUT_YOUR_PART_NAME_HERE"
    SceneObject *p_part = mp_root->FindDescendantByName("Building");
    if (!p_part)
        return;

    // Get Transfrom Property object
    Transform* p_trans = p_part->GetComponent<Transform>();
    if (p_trans)
    {
        // Get Rotation Vector and set z-rotation as the specifed angle
        glm::vec3 rotate_vec = p_trans->Rotation.Get();
        rotate_vec.z = angle;
        p_trans->Rotation.Set(rotate_vec);
    }
    ////////////////////////////////////////////////////////////////////
}

/*
void CustomProp::OnYourPropChanged(double your_prop)
{
    if (mp_root == NULL)
        return;

    std::cout << "Your Prop = " << your_prop << std::endl;
}
*/

void CustomProp::OnRAFChanged(double raf) {
    if (mp_root == NULL) {
        return;
    }

    // Control arm
    SceneObject* left_arm = mp_root->FindDescendantByName("LASphere");
    if (!left_arm) {
        return;
    }
    Transform* la_trans = left_arm->GetComponent<Transform>();
    if (la_trans) {
        glm::vec3 translate_vec = la_trans->Translation.Get();
        double k = raf > 50 ? 0 : 1 - raf/50;
        translate_vec.x = k*(-0.7);
        la_trans->Translation.Set(translate_vec);
    }

    // Control bow string
    SceneObject* string_top = mp_root->FindDescendantByName("StringTop");
    SceneObject* string_bot = mp_root->FindDescendantByName("StringBot");
    if (!string_top || ! string_bot) {
        return;
    }
    Transform* st_trans = string_top->GetComponent<Transform>();
    Transform* sb_trans = string_bot->GetComponent<Transform>();
    if (st_trans && sb_trans) {
        glm::vec3 st_rotate_vec = st_trans->Rotation.Get();
        glm::vec3 sb_rotate_vec = sb_trans->Rotation.Get();
        double k = raf > 50 ? (raf > 70 ? 1 : (raf-50)/20) : 1 - raf/50;
        st_rotate_vec.z = k*15 - 10;
        sb_rotate_vec.z = (1-k)*15 + 10;
        st_trans->Rotation.Set(st_rotate_vec);
        sb_trans->Rotation.Set(sb_rotate_vec);
    }

    // Control arrow
    SceneObject* arrow = mp_root->FindDescendantByName("AMesh");
    if (!arrow) {
        return;
    }
    Transform* arrow_trans = arrow->GetComponent<Transform>();
    if (arrow_trans) {
        glm::vec3 translate_vec = arrow_trans->Translation.Get();
        double k = raf > 50 ? -(raf-50)/5 : raf/50;
        translate_vec.y = k*(0.5);
        arrow_trans->Translation.Set(translate_vec);
    }
}
