import React from 'react';
import { Field } from 'formik';
import { EffectForm, labelColumns, fieldColumns } from '../EffectForms';
import Grid from '@mui/material/Grid';
import { effectConfig, EffectType } from '../../../types/index';
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
        <Grid container spacing={2}>
            <EffectForm effect={effect} onSubmit={onSubmit} onDelete={onDelete}>
                <Grid item xs={labelColumns}>
                    <label htmlFor="speed">Speed</label>
                </Grid>
                <Grid item xs={fieldColumns}>
                    <Field id="speed" name="speed" type="number" placeholder="Enter value (1-5)" />
                </Grid>
            </EffectForm>
        </Grid>
    );
};
