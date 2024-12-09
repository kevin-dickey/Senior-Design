import React from 'react';
import { ErrorMessage, Field } from 'formik';
import { EffectForm, labelColumns, fieldColumns } from '../EffectForms';
import Grid from '@mui/material/Grid';
import { EffectType } from '../../../types/index';
import { Effect } from '../../serialization/Effect';
interface EffectFormProps {
    effect: Effect;
    onSubmit: (effect: any) => void;
    onDelete?: (effectId: number) => void;
}

export const EffectsFormPresentational: React.FC<EffectFormProps> = ({ effect, onSubmit, onDelete }) => {
    const labelColumns = 7;
    const fieldColumns = 12 - labelColumns;
    return (
        <EffectForm effect={effect} onSubmit={onSubmit} onDelete={onDelete}>
            <Grid item xs={labelColumns}>
                <label htmlFor="speed">Speed</label>
            </Grid>
            <Grid item xs={fieldColumns}>
                <Field id="speed"
                    name="speed"
                    type="number"
                    placeholder="Enter value (1-5)"
                    min="0"
                    max="5"
                    step="1"
                    validate={(value: string | number | undefined) => {
                        if (!value) return "Speed is required";
                        if (typeof value === "number" && value < 0) return "Speed must be at least 1";
                        if (!Number.isInteger(Number(value))) return "Speed must be an integer";
                        if (typeof value === "number" && value > 5) return "Speed must be less than 5";
                        return undefined;
                    }}
                />
                <ErrorMessage name="speed" component="div" />
            </Grid>
        </EffectForm>
    );
};
