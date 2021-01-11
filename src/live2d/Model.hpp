#ifndef LIVE2D_MODEL_HPP
#define LIVE2D_MODEL_HPP

#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Type/csmRectF.hpp>
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>

#include "TextureManager.hpp"


class Model : public Csm::CubismUserModel {
public:
  /**
   * Default constructor/destructor
   */
  Model();
  virtual ~Model();

  /**
   * @brief Load from a model3.json file
   * 
   * @param[in] dir The directory the json is located in
   * @param[in] filename The filename of the json in the directory
   * @param[in] texture_manager The texture manager used to keep track of loaded textures
   */
  void load_assets(const Csm::csmChar* dir, const Csm::csmChar* filename, TextureManager* texture_manager);

  /**
   * @brief Reload internal renderer, refreshing all textures
   * 
   * @param[in] texture_manager The texture manager that keeps track of loaded textures
   */
  void reload_renderer(TextureManager* texture_manager);
  
  void update();

  /**
   * @brief Render the model onto the internal renderer using the provided projection matrix
   * 
   * @param[in] matrix The view projection matrix to use
   */
  void draw(Csm::CubismMatrix44& matrix);

  Csm::CubismMotionQueueEntryHandle start_motion(
    const Csm::csmChar* group,
    Csm::csmInt32 motion_num,
    Csm::csmInt32 priority,
    Csm::ACubismMotion::FinishedMotionCallback on_motion_finished = NULL
    );

  Csm::CubismMotionQueueEntryHandle start_random_motion(
    const Csm::csmChar* group,
    Csm::csmInt32 priority,
    Csm::ACubismMotion::FinishedMotionCallback on_motion_finished = NULL
  );

  void set_expression(const Csm::csmChar* expression);
  void set_random_expression();

  virtual void motion_event_fired(const Live2D::Cubism::Framework::csmString& event_value);

  /**
   * @brief Tests if a given coordinate is inside an aera of the model
   */
  virtual Csm::csmBool hit_test(const Csm::csmChar* area_name, Csm::csmFloat32 x, Csm::csmFloat32 y);

  /**
   * @brief Returns a reference to the internal render buffer of the model
   */
  Csm::Rendering::CubismOffscreenFrame_OpenGLES2& get_render_buffer();

protected:
  void do_draw();
private:
  void setup_model(Csm::ICubismModelSetting* settings);
  void setup_textures(TextureManager* texture_manager);
  void preload_motion_group(const Csm::csmChar* group);
  void release_motion_group(const Csm::csmChar* group);

  /**
   * @brief Release all motion groups
   */
  void release_motions();

  /**
   * @brief Release all expression motion groups
   */
  void release_expressions();

  Csm::ICubismModelSetting* _modelSetting; ///< モデルセッティング情報
  Csm::csmString _modelHomeDir; ///< モデルセッティングが置かれたディレクトリ
  Csm::csmFloat32 _userTimeSeconds; ///< デルタ時間の積算値[秒]
  Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///< モデルに設定されたまばたき機能用パラメータID
  Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds; ///< モデルに設定されたリップシンク機能用パラメータID
  Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions; ///< 読み込まれているモーションのリスト
  Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions; ///< 読み込まれている表情のリスト
  Csm::csmVector<Csm::csmRectF> _hitArea;
  Csm::csmVector<Csm::csmRectF> _userArea;
  const Csm::CubismId* _idParamAngleX; ///< パラメータID: ParamAngleX
  const Csm::CubismId* _idParamAngleY; ///< パラメータID: ParamAngleX
  const Csm::CubismId* _idParamAngleZ; ///< パラメータID: ParamAngleX
  const Csm::CubismId* _idParamBodyAngleX; ///< パラメータID: ParamBodyAngleX
  const Csm::CubismId* _idParamEyeBallX; ///< パラメータID: ParamEyeBallX
  const Csm::CubismId* _idParamEyeBallY; ///< パラメータID: ParamEyeBallXY

  Csm::Rendering::CubismOffscreenFrame_OpenGLES2  _renderBuffer;   ///< フレームバッファ以外の描画先
};

#endif /* LIVE2D_MODEL_HPP */