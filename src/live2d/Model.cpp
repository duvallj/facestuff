#include "Model.hpp"

#include <fstream>
#include <vector>
#include <CubismModelSettingJson.hpp>
#include <Motion/CubismMotion.hpp>
#include <Physics/CubismPhysics.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Rendering/OpenGL/CubismRenderer_OpenGLES2.hpp>
#include <Utils/CubismString.hpp>
#include <Id/CubismIdManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>

#include "Definitions.hpp"
#include "Util.hpp"

Csm::csmByte* create_buffer(const Csm::csmChar* path, Csm::csmSizeInt* size) {
  if (LAppDefinitions::DebugLogEnable) {
    LAppUtil::print_log("[APP] creating buffer %s ", path);
  }
  return LAppUtil::load_file_as_bytes(path, size);
}

void delete_buffer(Csm::csmByte* buffer, const Csm::csmChar* path = "") {
  if (LAppDefinitions::DebugLogEnable) {
    LAppUtil::print_log("[APP] deleting buffer %s ", path);
  }
  LAppUtil::release_bytes(buffer);
}

Model::Model()
  : CubismUserModel(),
  _modelSetting(NULL),
  _userTimeSeconds(0.0f)
{
  if (LAppDefinitions::DebugLogEnable) {
    _debugMode = true;
  }

  _idParamAngleX = Live2D::Cubism::Framework::CubismFramework::GetIdManager()->GetId(Live2D::Cubism::Framework::DefaultParameterId::ParamAngleX);
  _idParamAngleY = Live2D::Cubism::Framework::CubismFramework::GetIdManager()->GetId(Live2D::Cubism::Framework::DefaultParameterId::ParamAngleY);
  _idParamAngleZ = Live2D::Cubism::Framework::CubismFramework::GetIdManager()->GetId(Live2D::Cubism::Framework::DefaultParameterId::ParamAngleZ);
  _idParamBodyAngleX = Live2D::Cubism::Framework::CubismFramework::GetIdManager()->GetId(Live2D::Cubism::Framework::DefaultParameterId::ParamBodyAngleX);
  _idParamEyeBallX = Live2D::Cubism::Framework::CubismFramework::GetIdManager()->GetId(Live2D::Cubism::Framework::DefaultParameterId::ParamEyeBallX);
  _idParamEyeBallY = Live2D::Cubism::Framework::CubismFramework::GetIdManager()->GetId(Live2D::Cubism::Framework::DefaultParameterId::ParamEyeBallY);
}

Model::~Model() {
  _renderBuffer.DestroyOffscreenFrame();

  release_motions();
  release_expressions();

  for (Csm::csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++) {
    const Csm::csmChar* group = _modelSetting->GetMotionGroupName(i);
    release_motion_group(group);
  }

  delete _modelSetting;
}

void Model::load_assets(const Csm::csmChar* dir, const Csm::csmChar* fileName) {
  if (dir == NULL) {
    if (LAppDefinitions::DebugLogEnable) {
      LAppUtil::print_log("Error: NULL dir passed to Model::load_assets");
    }
    return;
  }
  if (fileName == NULL) {
    if (LAppDefinitions::DebugLogEnable) {
      LAppUtil::print_log("Error: NULL fileName passed to Model::load_assets");
    }
    return;
  }

  _modelHomeDir = dir;
  if (LAppDefinitions::DebugLogEnable) {
    LAppUtil::print_log("[APP] loading model settings: %s", fileName);
  }

  Csm::csmSizeInt size;
  const Csm::csmString path = Csm::csmString(dir) + fileName;

  Csm::csmByte* buffer = create_buffer(path.GetRawString(), &size);
  if (buffer == NULL) {
    if (LAppDefinitions::DebugLogEnable) {
      LAppUtil::print_log("Error reading model settings file %s", path.GetRawString());
    }
    return;
  }

  Live2D::Cubism::Framework::ICubismModelSetting* settings = new Live2D::Cubism::Framework::CubismModelSettingJson(buffer, size);
  delete_buffer(buffer, path.GetRawString());

  setup_model(settings);
  CreateRenderer();
  setup_textures();
}

void Model::setup_model(Live2D::Cubism::Framework::ICubismModelSetting* settings) {
  _updating = true;
  _initialized = false;

  _modelSetting = settings;

  Csm::csmByte* buffer;
  Csm::csmSizeInt size;

  //Cubism Model
  if (strcmp(_modelSetting->GetModelFileName(), "") != 0)
  {
    Csm::csmString path = _modelSetting->GetModelFileName();
    path = _modelHomeDir + path;

    if (_debugMode)
    {
      LAppUtil::print_log("[APP]create model: %s", settings->GetModelFileName());
    }

    buffer = create_buffer(path.GetRawString(), &size);
    LoadModel(buffer, size);
    delete_buffer(buffer, path.GetRawString());
  }

  //Expression
  if (_modelSetting->GetExpressionCount() > 0)
  {
    const Csm::csmInt32 count = _modelSetting->GetExpressionCount();
    for (Csm::csmInt32 i = 0; i < count; i++)
    {
      Csm::csmString name = _modelSetting->GetExpressionName(i);
      Csm::csmString path = _modelSetting->GetExpressionFileName(i);
      path = _modelHomeDir + path;

      buffer = create_buffer(path.GetRawString(), &size);
      if (buffer != NULL) {
        Live2D::Cubism::Framework::ACubismMotion* motion = LoadExpression(buffer, size, name.GetRawString());

        if (_expressions[name] != NULL)
        {
          Live2D::Cubism::Framework::ACubismMotion::Delete(_expressions[name]);
          _expressions[name] = NULL;
        }
        _expressions[name] = motion;
      }

      delete_buffer(buffer, path.GetRawString());
    }
  }

  //Physics
  if (strcmp(_modelSetting->GetPhysicsFileName(), "") != 0)
  {
    Csm::csmString path = _modelSetting->GetPhysicsFileName();
    path = _modelHomeDir + path;

    buffer = create_buffer(path.GetRawString(), &size);
    if (buffer != NULL) {
      LoadPhysics(buffer, size);
    }
    delete_buffer(buffer, path.GetRawString());
  }

  //Pose
  if (strcmp(_modelSetting->GetPoseFileName(), "") != 0)
  {
    Csm::csmString path = _modelSetting->GetPoseFileName();
    path = _modelHomeDir + path;

    buffer = create_buffer(path.GetRawString(), &size);
    if (buffer != NULL) {
      LoadPose(buffer, size);
    }
    delete_buffer(buffer, path.GetRawString());
  }

  //EyeBlink
  if (_modelSetting->GetEyeBlinkParameterCount() > 0)
  {
    _eyeBlink = Live2D::Cubism::Framework::CubismEyeBlink::Create(_modelSetting);
  }

  //Breath
  {
    _breath = Live2D::Cubism::Framework::CubismBreath::Create();

    Csm::csmVector<Live2D::Cubism::Framework::CubismBreath::BreathParameterData> breathParameters;

    breathParameters.PushBack(Live2D::Cubism::Framework::CubismBreath::BreathParameterData(_idParamAngleX, 0.0f, 15.0f, 6.5345f, 0.5f));
    breathParameters.PushBack(Live2D::Cubism::Framework::CubismBreath::BreathParameterData(_idParamAngleY, 0.0f, 8.0f, 3.5345f, 0.5f));
    breathParameters.PushBack(Live2D::Cubism::Framework::CubismBreath::BreathParameterData(_idParamAngleZ, 0.0f, 10.0f, 5.5345f, 0.5f));
    breathParameters.PushBack(Live2D::Cubism::Framework::CubismBreath::BreathParameterData(_idParamBodyAngleX, 0.0f, 4.0f, 15.5345f, 0.5f));
    breathParameters.PushBack(Live2D::Cubism::Framework::CubismBreath::BreathParameterData(Live2D::Cubism::Framework::CubismFramework::GetIdManager()->GetId(Live2D::Cubism::Framework::DefaultParameterId::ParamBreath), 0.5f, 0.5f, 3.2345f, 0.5f));

    _breath->SetParameters(breathParameters);
  }

  //UserData
  if (strcmp(_modelSetting->GetUserDataFile(), "") != 0)
  {
    Csm::csmString path = _modelSetting->GetUserDataFile();
    path = _modelHomeDir + path;
    buffer = create_buffer(path.GetRawString(), &size);
    if (buffer != NULL) {
      LoadUserData(buffer, size);
    }
    delete_buffer(buffer, path.GetRawString());
  }

  // EyeBlinkIds
  {
    Csm::csmInt32 eyeBlinkIdCount = _modelSetting->GetEyeBlinkParameterCount();
    for (Csm::csmInt32 i = 0; i < eyeBlinkIdCount; ++i)
    {
      _eyeBlinkIds.PushBack(_modelSetting->GetEyeBlinkParameterId(i));
    }
  }

  // LipSyncIds
  {
    Csm::csmInt32 lipSyncIdCount = _modelSetting->GetLipSyncParameterCount();
    for (Csm::csmInt32 i = 0; i < lipSyncIdCount; ++i)
    {
      _lipSyncIds.PushBack(_modelSetting->GetLipSyncParameterId(i));
    }
  }

  //Layout
  Csm::csmMap<Csm::csmString, Csm::csmFloat32> layout;
  _modelSetting->GetLayoutMap(layout);
  _modelMatrix->SetupFromLayout(layout);

  _model->SaveParameters();

  for (Csm::csmInt32 i = 0; i < _modelSetting->GetMotionGroupCount(); i++)
  {
    const Csm::csmChar* group = _modelSetting->GetMotionGroupName(i);
    preload_motion_group(group);
  }

  _motionManager->StopAllMotions();

  _updating = false;
  _initialized = true;
}

void Model::preload_motion_group(const Csm::csmChar* group) {
  const Csm::csmInt32 count = _modelSetting->GetMotionCount(group);

  for (Csm::csmInt32 i = 0; i < count; i++) {
    Csm::csmString name = Live2D::Cubism::Framework::Utils::CubismString::GetFormatedString("%s_%d", group, i);
    Csm::csmString path = _modelSetting->GetMotionFileName(group, i);
    path = _modelHomeDir + path;

    if (_debugMode) {
      LAppUtil::print_log("[APP] loading motion %s => [%s_%d]", path.GetRawString(), group, i);
    }

    Csm::csmByte* buffer;
    Csm::csmSizeInt size;
    buffer = create_buffer(path.GetRawString(), &size);
    if (buffer != NULL) {
      Live2D::Cubism::Framework::CubismMotion* tmpMotion = static_cast<Live2D::Cubism::Framework::CubismMotion*>(LoadMotion(buffer, size, name.GetRawString()));
      
      Csm::csmFloat32 fadeTime = _modelSetting->GetMotionFadeInTimeValue(group, i);
      if (fadeTime >= 0.0f) {
        tmpMotion->SetFadeInTime(fadeTime);
      }

      fadeTime = _modelSetting->GetMotionFadeOutTimeValue(group, i);
      if (fadeTime >= 0.0f) {
        tmpMotion->SetFadeOutTime(fadeTime);
      }

      tmpMotion->SetEffectIds(_eyeBlinkIds, _lipSyncIds);

      if (_motions[name] != NULL) {
        Live2D::Cubism::Framework::ACubismMotion::Delete(_motions[name]);
      }
      _motions[name] = tmpMotion;
    }

    delete_buffer(buffer, path.GetRawString());
  }
}

void Model::release_motion_group(const Csm::csmChar* group) {
  const Csm::csmInt32 count = _modelSetting->GetMotionCount(group);
  for (Csm::csmInt32 i = 0; i < count; i++) {
    Csm::csmString voice = _modelSetting->GetMotionSoundFileName(group, i);
    // This function doesn't seem to do anything after this???
  }
}

void Model::release_motions() {
  for (auto iter = _motions.Begin(); iter != _motions.End(); ++iter) {
    Live2D::Cubism::Framework::ACubismMotion::Delete(iter->Second);
  }

  _motions.Clear();
}

void Model::release_expressions() {
  for (auto iter = _expressions.Begin(); iter != _expressions.End(); ++iter) {
    Live2D::Cubism::Framework::ACubismMotion::Delete(iter->Second);
  }

  _expressions.Clear();
}

void Model::update() {
  const Csm::csmFloat32 delta_time_seconds = LAppUtil::get_delta_time();
  _userTimeSeconds += delta_time_seconds;

  _dragManager->Update(delta_time_seconds);
  _dragX = _dragManager->GetX();
  _dragY = _dragManager->GetY();

  Csm::csmBool motion_updated = false;

  _model->LoadParameters();
  if (_motionManager->IsFinished()) {
    start_random_motion(LAppDefinitions::MotionGroupIdle, LAppDefinitions::PriorityIdle);
  }
  else {
    motion_updated = _motionManager->UpdateMotion(_model, delta_time_seconds);
  }
  _model->SaveParameters();

  if (!motion_updated) {
    // When idling, update the eye blink interval if it exists
    if (_eyeBlink != NULL) {
      _eyeBlink->UpdateParameters(_model, delta_time_seconds);
    }
  }

  if (_expressionManager != NULL) {
    _expressionManager->UpdateMotion(_model, delta_time_seconds);
  }

  // This is where we update where the model is looking based on where the drag currently is
  // TODO: repurpose to take in an arbitrary x,y from elsewhere?
  _model->AddParameterValue(_idParamAngleX, _dragX * 30);
  _model->AddParameterValue(_idParamAngleY, _dragY * 30);
  _model->AddParameterValue(_idParamAngleZ, _dragX * _dragY * -30); // magic wowow

  _model->AddParameterValue(_idParamBodyAngleX, _dragX * 10);

  _model->AddParameterValue(_idParamEyeBallX, _dragX);
  _model->AddParameterValue(_idParamEyeBallY, _dragY);

  if (_breath != NULL) {
    _breath->UpdateParameters(_model, delta_time_seconds);
  }

  if (_physics != NULL) {
    _physics->Evaluate(_model, delta_time_seconds);
  }

  if (_lipSync != NULL) {
    Csm::csmFloat32 value = 0; /* This would be a volume value read in from the system,
                                * We don't do that here, so just have the lips be closed */
    for (Csm::csmUint32 i = 0; i < _lipSyncIds.GetSize(); i++) {
      _model->AddParameterValue(_lipSyncIds[i], value, 0.8f);
    }
  }

  if (_pose != NULL) {
    _pose->UpdateParameters(_model, delta_time_seconds);
  }

  _model->Update();
}