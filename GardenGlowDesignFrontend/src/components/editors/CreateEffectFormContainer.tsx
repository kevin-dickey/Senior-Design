import React from 'react';
import { Effect } from '../serialization/Effect';
import {
    CreateRainbowEffectFormContainer
} from './RainbowEffectForm/CreateRainbowEffectFormContainer';
import {CreateEffectsFormContainer} from './EffectsForm/CreateEffectsFormContainer'
import { CreateRippleEffectFormContainer } from "./RippleEffectForm/CreateRippleEffectFormContainer";
import { EffectType } from '../../types';

interface CreateEffectFormContainerProps {
    effectType: EffectType;
    onSubmit: (effect: Effect) => void;
}

// TODO: This can probably be collapsed into the EditEffectForm container which will handle both
//  creation and editing.
export const CreateEffectFormContainer: React.FC<CreateEffectFormContainerProps> =
    ({ effectType, onSubmit }) => {
        switch (effectType) {
            case EffectType.rainbow:
                return (
                    <CreateRainbowEffectFormContainer onSubmit={onSubmit} />
                );
            case EffectType.ripple:
                return (
                    <CreateRippleEffectFormContainer onSubmit={onSubmit} />
                );
            case EffectType.pumpkinRainbow:
            case EffectType.pumpkinRipple:
            case EffectType.ghostRainbow:
            case EffectType.ghostRipple:
            case EffectType.pumpkinGhostRainbow:
            case EffectType.pumpkinGhostRipple:
            case EffectType.snowflake:
            case EffectType.snowman:
            case EffectType.christmasTree:
            case EffectType.candyCane:
                console.log("case selected");
                return <CreateEffectsFormContainer effectType={effectType} onSubmit={onSubmit}  />;
            default:
                return null;
        }
    };