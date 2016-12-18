//
//  LoadingScene.cpp
//  cocos2d_tests
//
//  Created by minggo on 10/12/16.
//
//

#include "LoadingScene.h"
#include "Utils.h"

using namespace cocos2d;

LoadingScene* LoadingScene::create(CCScene* newScene)
{
    LoadingScene* scene = new LoadingScene(newScene);
    scene->autorelease();
    return scene;
}

LoadingScene::LoadingScene(CCScene* newScene)
: _index(0)
, _newScene(newScene)
, _endOfMd5(false)
{
    _newScene->retain();
    this->scheduleUpdate();
    
//    this->schedule(CC_CALLBACK_1(LoadingScene::replaceScene, this), 9, "replaceScene");
    
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    CCSize size = CCDirector::sharedDirector()->getVisibleSize();
    CCLabelTTF* label = CCLabelTTF::create("loading...", "", 15);
    label->setPosition(ccp(origin.x + size.width/2, origin.y + size.height/2));
    label->setTag(100);
    this->addChild(label);

    const char* bitfiles[] = {
        "config/monster_res.bit",
        "config/star_accumulate_points.bit",
        "config/star_achievement.bit",
        "config/star_act_game_award.bit",
        "config/star_act_game_combat_param.bit",
        "config/star_activity.bit",
        "config/star_activity_extra.bit",
        "config/star_ai.bit",
        "config/star_announcement.bit",
        "config/star_assit_fighter_stop.bit",
        "config/star_background.bit",
        "config/star_batter_add.bit",
        "config/star_boss_challenge.bit",
        "config/star_boxconf.bit",
        "config/star_bullet_buf.bit",
        "config/star_bullet_buf_fighter_patch.bit",
        "config/star_bullet_buf_hero.bit",
        "config/star_bullet_buf_match_eliminate.bit",
        "config/star_bullet_preview_zoom.bit",
        "config/star_bullet_skill.bit",
        "config/star_bulletbufclient.bit",
        "config/star_challenge_target.bit",
        "config/star_cheat_tolerance_increase.bit",
        "config/star_cheat_tolerance_parameter_new.bit",
        "config/star_cheat_turning_factor.bit",
        "config/star_client_func_call_limit.bit",
        "config/star_combat_score_limit.bit",
        "config/star_compete_cheat_parameter.bit",
        "config/star_compete_enemy_fighter_hp_rate.bit",
        "config/star_compete_fireball_hurt.bit",
        "config/star_compete_hits_fireball.bit",
        "config/star_compete_hosted_hurt_rate.bit",
        "config/star_compete_hosted_time_rate.bit",
        "config/star_compete_huluwa_random.bit",
        "config/star_compete_huluwa_resource.bit",
        "config/star_compete_military.bit",
        "config/star_compete_monster_resource.bit",
        "config/star_compete_pk_extra_reward.bit",
        "config/star_compete_pk_force_match_fix.bit",
        "config/star_compete_pk_score_fix.bit",
        "config/star_compete_pk_score_init.bit",
        "config/star_compete_pk_score_point.bit",
        "config/star_compete_score.bit",
        "config/star_compete_skill_check.bit",
        "config/star_compete_total_check.bit",
        "config/star_condition.bit",
        "config/star_content.bit",
        "config/star_debuff_parameters.bit",
        "config/star_delay_rank.bit",
        "config/star_drop_case.bit",
        "config/star_duplicate.bit",
        "config/star_effects.bit",
        "config/star_element_restriction.bit",
        "config/star_eliminate_add.bit",
        "config/star_equip.bit",
        "config/star_equip_illustrations.bit",
        "config/star_equip_suite.bit",
        "config/star_exempt_compress_resource.bit",
        "config/star_expedition_award.bit",
        "config/star_expedition_bullet_hurt.bit",
        "config/star_expedition_combat_param.bit",
        "config/star_expedition_comm_cfg.bit",
        "config/star_expedition_game_config.bit",
        "config/star_expedition_level_param.bit",
        "config/star_expedition_map.bit",
        "config/star_expedition_rank_show.bit",
        "config/star_expedition_reset_cost.bit",
        "config/star_expedition_sweep.bit",
        "config/star_fighter_buf.bit",
        "config/star_fighter_evolve_info.bit",
        "config/star_fighter_resources.bit",
        "config/star_fit.bit",
        "config/star_fit_income.bit",
        "config/star_fps_rank.bit",
        "config/star_game_benefit.bit",
        "config/star_game_event.bit",
        "config/star_game_formation.bit",
        "config/star_game_level.bit",
        "config/star_game_levelrnd.bit",
        "config/star_game_model.bit",
        "config/star_game_model_func.bit",
        "config/star_global.bit",
        "config/star_gm_list.bit",
        "config/star_guild_level.bit",
        "config/star_guild_match_award.bit",
        "config/star_guild_position.bit",
        "config/star_guild_shop_lottery_item.bit",
        "config/star_guild_shop_pos_refresh.bit",
        "config/star_guild_shop_pos_refresh_consume.bit",
        "config/star_guild_shop_upgrade.bit",
        "config/star_guild_skill.bit",
        "config/star_guild_stage_award.bit",
        "config/star_guild_task.bit",
        "config/star_hand_book.bit",
        "config/star_hand_book_cond.bit",
        "config/star_illustrations.bit",
        "config/star_item.bit",
        "config/star_item_daily_limit.bit",
        "config/star_item_recycle.bit",
        "config/star_item_reward.bit",
        "config/star_itemshop.bit",
        "config/star_leap_combat_to_level.bit",
        "config/star_leap_e_to_level.bit",
        "config/star_leap_level_to_skill.bit",
        "config/star_lev_xp.bit",
        "config/star_loadingtips.bit",
        "config/star_lottery.bit",
        "config/star_lottery_item_filter.bit",
        "config/star_lottery_probab.bit",
        "config/star_lottery_show.bit",
        "config/star_lottery_type.bit",
        "config/star_map_background_conf.bit",
        "config/star_match_eliminate_reward.bit",
        "config/star_match_erea.bit",
        "config/star_match_rank_award.bit",
        "config/star_match_remind.bit",
        "config/star_match_schedule.bit",
        "config/star_match_season.bit",
        "config/star_match_stage_award.bit",
        "config/star_match_support_reward.bit",
        "config/star_monster.bit",
        "config/star_monster_fireball.bit",
        "config/star_monster_group.bit",
        "config/star_monster_hero.bit",
        "config/star_monster_match_eliminate.bit",
        "config/star_monster_movement.bit",
        "config/star_monster_path.bit",
        "config/star_new_expedition_resources.bit",
        "config/star_pet.bit",
        "config/star_pet_collision.bit",
        "config/star_pet_hatch.bit",
        "config/star_pet_motion.bit",
        "config/star_pet_resources.bit",
        "config/star_pet_skill_init.bit",
        "config/star_pet_skill_reset.bit",
        "config/star_pilot_dub.bit",
        "config/star_pk_field.bit",
        "config/star_points_icon.bit",
        "config/star_pri_present.bit",
        "config/star_property.bit",
        "config/star_random_event_strategy.bit",
        "config/star_receptionglobalcfg.bit",
        "config/star_rewardmonth.bit",
        "config/star_scene_skill.bit",
        "config/star_schedule.bit",
        "config/star_score.bit",
        "config/star_sell_shop.bit",
        "config/star_server_special_uin_list.bit",
        "config/star_share.bit",
        "config/star_shop_buy.bit",
        "config/star_show_off.bit",
        "config/star_skill.bit",
        "config/star_skill_parameters.bit",
        "config/star_sound.bit",
        "config/star_special_extra_limit.bit",
        "config/star_special_limit.bit",
        "config/star_stage_add.bit",
        "config/star_story_dialog.bit",
        "config/star_task.bit",
        "config/star_task_activity_reward.bit",
        "config/star_task_title_show.bit",
        "config/star_team_game_content.bit",
        "config/star_team_game_default_match_score.bit",
        "config/star_team_game_rank.bit",
        "config/star_team_skill_interrupt.bit",
        "config/star_thing_output_limit.bit",
        "config/star_vcoin_area.bit",
        "config/star_vip_config.bit",
        "config/star_vip_show.bit",
        "config/star_week_rank_award.bit"
    };
    for (int i = 0; i < sizeof(bitfiles)/sizeof(bitfiles[0]); ++i)
    {
        _bitfiles.push_back(bitfiles[i]);
    }

    const char* resfiles[] = {
        "res/animated-grossini1.plist",
        "res/animated-grossini10.plist",
        "res/animated-grossini11.plist",
        "res/animated-grossini12.plist",
        "res/animated-grossini13.plist",
        "res/animated-grossini14.plist",
        "res/animated-grossini15.plist",
        "res/animated-grossini16.plist",
        "res/animated-grossini17.plist",
        "res/animated-grossini18.plist",
        "res/animated-grossini19.plist",
        "res/animated-grossini2.plist",
        "res/animated-grossini20.plist",
        "res/animated-grossini21.plist",
        "res/animated-grossini22.plist",
        "res/animated-grossini23.plist",
        "res/animated-grossini24.plist",
        "res/animated-grossini25.plist",
        "res/animated-grossini26.plist",
        "res/animated-grossini27.plist",
        "res/animated-grossini28.plist",
        "res/animated-grossini29.plist",
        "res/animated-grossini3.plist",
        "res/animated-grossini30.plist",
        "res/animated-grossini31.plist",
        "res/animated-grossini32.plist",
        "res/animated-grossini33.plist",
        "res/animated-grossini34.plist",
        "res/animated-grossini35.plist",
        "res/animated-grossini36.plist",
        "res/animated-grossini37.plist",
        "res/animated-grossini38.plist",
        "res/animated-grossini39.plist",
        "res/animated-grossini4.plist",
        "res/animated-grossini40.plist",
        "res/animated-grossini41.plist",
        "res/animated-grossini42.plist",
        "res/animated-grossini5.plist",
        "res/animated-grossini6.plist",
        "res/animated-grossini7.plist",
        "res/animated-grossini8.plist",
        "res/animated-grossini9.plist"
    };

    for (int i = 0; i < sizeof(resfiles)/sizeof(resfiles[0]); ++i)
    {
        _resfiles.push_back(resfiles[i]);
    }

    // set fps to 30 for loading
    CCDirector::sharedDirector()->setAnimationInterval(1 / 30.0f);
}

void LoadingScene::replaceScene(float dt)
{
    CCDirector::sharedDirector()->replaceScene(_newScene);
    _newScene->release();
    _newScene = NULL;
}

void LoadingScene::update(float dt)
{
    // compute md5
    
    if (_index < _bitfiles.size())
    {
        computeMD5(_bitfiles[_index]);
        
        ++_index;
    }
    else
    {
        _index = 0;
        _endOfMd5 = true;
    }
    
    if (_endOfMd5)
    {
        // load  resources after computing md5
        if (_index < _resfiles.size())
        {
            for (int i = 0; i < 10 && _index < _resfiles.size(); ++i, ++_index)
                CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(_resfiles[_index].c_str());
        }
        else
        {
            replaceScene(0);
            // reset fps to 60 after loading
            CCDirector::sharedDirector()->setAnimationInterval(1 / 60.0);
        }
    }
}

void LoadingScene::computeMD5(const std::string& fileName)
{
    unsigned long size = 0;
    unsigned char* data = CCFileUtils::sharedFileUtils()->getFileData(fileName.c_str(), "rb", &size);
    myutils::md5((char*)data, size);
    CC_SAFE_DELETE_ARRAY(data);
}
