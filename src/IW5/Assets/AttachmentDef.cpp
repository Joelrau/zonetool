// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		AttachmentDef* IAttachmentDef::parse(const std::string& name, ZoneMemory* mem)
		{
			if (name == "cheytacscope")
			{
				auto scope = mem->Alloc<AttachmentDef>();
				auto base = DB_FindXAssetHeader(attachment, /*name.data()*/ "msrscope", 1).attachment;

				memcpy(scope, base, sizeof AttachmentDef);
				scope->szInternalName = mem->StrDup(name);

				for (int i = 0; i < 16; i++)
				{
					scope->worldModels[i] = nullptr;
					scope->viewModels[i] = nullptr;
				}

				scope->worldModels[0] = DB_FindXAssetHeader(xmodel, "weapon_cheytac_scope", 1).xmodel;
				scope->viewModels[0] = DB_FindXAssetHeader(xmodel, "viewmodel_cheytac_scope", 1).xmodel;

				ZONETOOL_INFO("msrscope -> cheytacscope!");

				return scope;
			}

			return nullptr;
		}

		void IAttachmentDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).attachment;
			}
		}

		void IAttachmentDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IAttachmentDef::load_depending(IZone* zone)
		{
			auto data = this->asset_;

			// if we didn't parse a custom asset and the asset is common: there is no point in writing it.
			// make a reference to the original asset instead.
			/*if (!this->m_parsed && AssetHandler::IsCommonAsset(this->type(), this->name()))
			{
				return;
			}*/

			for (std::int32_t i = 0; i < 16; i++)
			{
				if (data->worldModels && data->worldModels[i])
				{
					zone->add_asset_of_type(xmodel, data->worldModels[i]->name);
				}

				if (data->viewModels && data->viewModels[i])
				{
					zone->add_asset_of_type(xmodel, data->viewModels[i]->name);
				}
			}

			for (std::int32_t i = 0; i < 8; i++)
			{
				if (data->reticleViewModels && data->reticleViewModels[i])
				{
					zone->add_asset_of_type(xmodel, data->reticleViewModels[i]->name);
				}
			}

			if (data->ammogeneral)
			{
				if (data->ammogeneral->tracerType)
				{
					zone->add_asset_of_type(tracer, data->ammogeneral->tracerType->name);
				}
			}

			if (data->general)
			{
				if (data->general->reticleCenter)
				{
					zone->add_asset_of_type(material, data->general->reticleCenter->name);
				}

				if (data->general->reticleSide)
				{
					zone->add_asset_of_type(material, data->general->reticleSide->name);
				}
			}

			if (data->ui)
			{
				if (data->ui->ammoCounterIcon)
				{
					zone->add_asset_of_type(material, data->ui->ammoCounterIcon->name);
				}

				if (data->ui->dpadIcon)
				{
					zone->add_asset_of_type(material, data->ui->dpadIcon->name);
				}
			}

			if (data->adsOverlay)
			{
				if (data->adsOverlay->overlay.shader)
				{
					zone->add_asset_of_type(material, data->adsOverlay->overlay.shader->name);
				}

				if (data->adsOverlay->overlay.shaderLowRes)
				{
					zone->add_asset_of_type(material, data->adsOverlay->overlay.shaderLowRes->name);
				}

				if (data->adsOverlay->overlay.shaderEMP)
				{
					zone->add_asset_of_type(material, data->adsOverlay->overlay.shaderEMP->name);
				}

				if (data->adsOverlay->overlay.shaderEMPLowRes)
				{
					zone->add_asset_of_type(material, data->adsOverlay->overlay.shaderEMPLowRes->name);
				}
			}

			if (data->projectile)
			{
				if (data->projectile->projectileModel)
				{
					zone->add_asset_of_type(xmodel, data->projectile->projectileModel->name);
				}

				if (data->projectile->projExplosionEffect)
				{
					zone->add_asset_of_type(fx, data->projectile->projExplosionEffect->name);
				}

				if (data->projectile->projExplosionSound)
				{
					zone->add_asset_of_type(sound, data->projectile->projExplosionSound->aliasName);
				}

				if (data->projectile->projDudEffect)
				{
					zone->add_asset_of_type(fx, data->projectile->projDudEffect->name);
				}

				if (data->projectile->projDudSound)
				{
					zone->add_asset_of_type(sound, data->projectile->projDudSound->aliasName);
				}

				if (data->projectile->projTrailEffect)
				{
					zone->add_asset_of_type(fx, data->projectile->projTrailEffect->name);
				}

				if (data->projectile->projIgnitionEffect)
				{
					zone->add_asset_of_type(fx, data->projectile->projIgnitionEffect->name);
				}

				if (data->projectile->projIgnitionSound)
				{
					zone->add_asset_of_type(sound, data->projectile->projIgnitionSound->aliasName);
				}
			}
		}

		std::string IAttachmentDef::name()
		{
			return this->name_;
		}

		std::int32_t IAttachmentDef::type()
		{
			return attachment;
		}

		void IAttachmentDef::write(IZone* zone, ZoneBuffer* buf)
		{
			static_assert(sizeof(AttachmentDef) == 164);

			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->szInternalName = buf->write_str(this->name());

			if (data->szDisplayName)
			{
				dest->szDisplayName = buf->write_str(data->szDisplayName);
			}

			if (data->worldModels)
			{
				buf->align(3);
				auto models = buf->write(data->worldModels, 16);

				for (std::int32_t i = 0; i < 16; i++)
				{
					if (models[i])
					{
						models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, models[i]->name));
					}
				}

				ZoneBuffer::clear_pointer(&dest->worldModels);
			}

			if (data->viewModels)
			{
				buf->align(3);
				auto models = buf->write(data->viewModels, 16);

				for (std::int32_t i = 0; i < 16; i++)
				{
					if (models[i])
					{
						models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, models[i]->name));
					}
				}

				ZoneBuffer::clear_pointer(&dest->viewModels);
			}

			if (data->reticleViewModels)
			{
				buf->align(3);
				auto models = buf->write(data->reticleViewModels, 8);

				for (std::int32_t i = 0; i < 8; i++)
				{
					if (models[i])
					{
						models[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, models[i]->name));
					}
				}

				ZoneBuffer::clear_pointer(&dest->reticleViewModels);
			}
			static_assert(sizeof(*data->ammogeneral) == 24);
			static_assert(offsetof(AttAmmoGeneral, tracerType) == 16);
			if (data->ammogeneral)
			{
				buf->align(3);
				auto ammo = buf->write(data->ammogeneral);

				if (ammo->tracerType)
				{
					ammo->tracerType = reinterpret_cast<TracerDef*>(zone->get_asset_pointer(
						tracer, ammo->tracerType->name));
				}

				ZoneBuffer::clear_pointer(&dest->ammogeneral);
			}
			static_assert(sizeof(*data->sight) == 7);
			if (data->sight)
			{
				buf->align(3);
				buf->write(data->sight);
				ZoneBuffer::clear_pointer(&dest->sight);
			}
			static_assert(sizeof(*data->reload) == 2);
			if (data->reload)
			{
				buf->align(1);
				buf->write(data->reload);
				ZoneBuffer::clear_pointer(&dest->reload);
			}
			static_assert(sizeof(*data->addOns) == 2);
			if (data->addOns)
			{
				buf->align(1);
				buf->write(data->addOns);
				ZoneBuffer::clear_pointer(&dest->addOns);
			}
			static_assert(sizeof(*data->general) == 32);
			static_assert(offsetof(AttGeneral, reticleCenter) == 8);
			static_assert(offsetof(AttGeneral, reticleSide) == 12);
			if (data->general)
			{
				buf->align(3);
				auto general = buf->write(data->general);

				if (general->reticleCenter)
				{
					general->reticleCenter = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, general->reticleCenter->name));
				}

				if (general->reticleSide)
				{
					general->reticleSide = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, general->reticleSide->name));
				}

				ZoneBuffer::clear_pointer(&dest->general);
			}
			static_assert(sizeof(*data->aimAssist) == 12);
			if (data->aimAssist)
			{
				buf->align(3);
				buf->write(data->aimAssist);
				ZoneBuffer::clear_pointer(&dest->aimAssist);
			}
			static_assert(sizeof(*data->ammunition) == 24);
			if (data->ammunition)
			{
				buf->align(3);
				buf->write(data->ammunition);
				ZoneBuffer::clear_pointer(&dest->ammunition);
			}

			static_assert(sizeof(*data->damage) == 28);
			if (data->damage)
			{
				buf->align(3);
				buf->write(data->damage);
				ZoneBuffer::clear_pointer(&dest->damage);
			}
			
			static_assert(sizeof(*data->locationDamage) == 76);
			if (data->locationDamage)
			{
				buf->align(3);
				buf->write(data->locationDamage);
				ZoneBuffer::clear_pointer(&dest->locationDamage);
			}

			static_assert(sizeof(*data->idleSettings) == 24);
			if (data->idleSettings)
			{
				buf->align(3);
				buf->write(data->idleSettings);
				ZoneBuffer::clear_pointer(&dest->idleSettings);
			}

			static_assert(sizeof(*data->adsSettings) == 56);
			if (data->adsSettings)
			{
				buf->align(3);
				buf->write(data->adsSettings);
				ZoneBuffer::clear_pointer(&dest->adsSettings);
			}

			static_assert(sizeof(*data->adsSettingsMain) == 56);
			if (data->adsSettingsMain)
			{
				buf->align(3);
				buf->write(data->adsSettingsMain);
				ZoneBuffer::clear_pointer(&dest->adsSettingsMain);
			}

			static_assert(sizeof(*data->hipSpread) == 48);
			if (data->hipSpread)
			{
				buf->align(3);
				buf->write(data->hipSpread);
				ZoneBuffer::clear_pointer(&dest->hipSpread);
			}

			static_assert(sizeof(*data->gunKick) == 80);
			if (data->gunKick)
			{
				buf->align(3);
				buf->write(data->gunKick);
				ZoneBuffer::clear_pointer(&dest->gunKick);
			}

			static_assert(sizeof(*data->viewKick) == 40);
			if (data->viewKick)
			{
				buf->align(3);
				buf->write(data->viewKick);
				ZoneBuffer::clear_pointer(&dest->viewKick);
			}

			static_assert(sizeof(*data->adsOverlay) == 40);
			if (data->adsOverlay)
			{
				buf->align(3);
				auto overlay = buf->write(data->adsOverlay);

				if (overlay->overlay.shader)
				{
					overlay->overlay.shader = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, overlay->overlay.shader->name));
				}

				if (overlay->overlay.shaderEMP)
				{
					overlay->overlay.shaderEMP = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, overlay->overlay.shaderEMP->name));
				}

				if (overlay->overlay.shaderEMPLowRes)
				{
					overlay->overlay.shaderEMPLowRes = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, overlay->overlay.shaderEMPLowRes->name));
				}

				if (overlay->overlay.shaderLowRes)
				{
					overlay->overlay.shaderLowRes = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, overlay->overlay.shaderLowRes->name));
				}

				ZoneBuffer::clear_pointer(&dest->adsOverlay);
			}

			static_assert(sizeof(*data->ui) == 20);
			if (data->ui)
			{
				buf->align(3);
				auto ui = buf->write(data->ui);

				if (ui->ammoCounterIcon)
				{
					ui->ammoCounterIcon = reinterpret_cast<Material*>(zone->get_asset_pointer(
						material, ui->ammoCounterIcon->name));
				}

				if (ui->dpadIcon)
				{
					ui->dpadIcon = reinterpret_cast<Material*>(zone->get_asset_pointer(material, ui->dpadIcon->name));
				}

				ZoneBuffer::clear_pointer(&dest->ui);
			}

			static_assert(sizeof(*data->rumbles) == 8);
			if (data->rumbles)
			{
				buf->align(3);
				auto rumbles = buf->write(data->rumbles);

				if (rumbles->fireRumble)
				{
					rumbles->fireRumble = buf->write_str(rumbles->fireRumble);
				}

				if (rumbles->meleeImpactRumble)
				{
					rumbles->meleeImpactRumble = buf->write_str(rumbles->meleeImpactRumble);
				}

				ZoneBuffer::clear_pointer(&dest->rumbles);
			}

			static_assert(sizeof(*data->projectile) == 92);
			if (data->projectile)
			{
				#define WEAPON_SOUND_CUSTOM(__field__) \
				if (data->projectile->__field__) \
				{ \
					auto ptr = -1; \
					buf->align(3); \
					buf->write(&ptr); \
					buf->write_str(data->projectile->__field__->name); \
					ZoneBuffer::clear_pointer(&projectile->__field__); \
				}

				buf->align(3);
				auto projectile = buf->write(data->projectile);

				if (projectile->projectileModel)
				{
					projectile->projectileModel = reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, projectile->projectileModel->name));
				}

				if (projectile->projExplosionEffect)
				{
					projectile->projExplosionEffect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(fx, projectile->projExplosionEffect->name));
				}

				if (projectile->projExplosionSound)
				{
					WEAPON_SOUND_CUSTOM(projExplosionSound);
				}

				if (projectile->projDudEffect)
				{
					projectile->projDudEffect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(fx, projectile->projDudEffect->name));
				}

				if (projectile->projDudSound)
				{
					WEAPON_SOUND_CUSTOM(projDudSound);
				}

				if (projectile->projTrailEffect)
				{
					projectile->projTrailEffect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(fx, projectile->projTrailEffect->name));
				}

				if (projectile->projIgnitionEffect)
				{
					projectile->projIgnitionEffect = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(fx, projectile->projIgnitionEffect->name));
				}

				if (projectile->projIgnitionSound)
				{
					WEAPON_SOUND_CUSTOM(projIgnitionSound);
				}

				ZoneBuffer::clear_pointer(&dest->projectile);

				#undef WEAPON_SOUND_CUSTOM
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IAttachmentDef::dump(AttachmentDef* asset)
		{
			//const auto data = asset->ToJson();

			//std::string path = "attachments\\mp\\"s + asset->szInternalName;
			//std::string json = data.dump(4);

			//auto file = FileSystem::FileOpen(path, "w"s);
			//fwrite(json.data(), json.size(), 1, file);
			//FileSystem::FileClose(file);
		}
	}
}
