import React from "react";
import {Effect} from "../serialization/Effect";
import * as Yup from "yup";
import {EffectSchema} from "./EffectForms";

const effectsListSchema = Yup.array().of(EffectSchema).test(
    'no-overlapping-effects',
    'Effects should not overlap',
    (effects, context) => {
        for (let i = 0; i < effects!.length; i++) {
            for (let j = i + 1; j < effects!.length; j++) {
                const effect1 = effects![i];
                const effect2 = effects![j];

                const effect1End = effect1.startTimeMs + effect1.durationMs;
                const effect2End = effect2.startTimeMs + effect2.durationMs;

                const TwoOverOne = (effect1.startTimeMs < effect2End && effect1End > effect2.startTimeMs);
                const OneOverTwo = (effect2.startTimeMs < effect1End && effect2End > effect1.startTimeMs);
                // Mutually exclusive
                if (TwoOverOne || OneOverTwo) {
                    const message = TwoOverOne ?
                        `Effect ${effect2.name} starts before ${effect1.name} ends.` :
                        `Effect ${effect1.name} starts before ${effect2.name} ends.`;
                    context.createError({
                        path: `effects[${i}][${j}]`,
                        message: message,
                    })
                    return false;
                }
            }
        }
        return true;
    }
);

export const validateEffects = async (effects: Effect[]) => {
    try {
        await effectsListSchema.validate(effects, {abortEarly: false});
        return [];
    } catch (err: any) {
        return err.inner.map((error: any) => ({
            message: error.message,
            path: error.path,
        }));
    }
};

interface EffectListProps {
    effects: Effect[];
    onEffectSelected: any;
    // This should be the set function from useState in Configuration.tsx
}

export const EffectList: React.FC<EffectListProps> = ({effects, onEffectSelected}) => {
    const sortedEffects = effects.sort((a, b) => a.startTimeMs - b.startTimeMs);

    return (
        <ul>
            {sortedEffects.map((effect, index) => (
                <li key={index} onClick={() => onEffectSelected(effect.id)}>
                    {effect.name}
                </li>
            ))}
        </ul>
    );
}