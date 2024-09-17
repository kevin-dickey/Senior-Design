import React from 'react';
import {Field} from 'formik';
import {EffectForm} from '../EffectForms';
import {Effect} from '../../serialization/Effect';
import Grid from "@mui/material/Grid";

interface RippleEffectFormProps {
    effect: Effect;
    onSubmit: (effect: Effect) => void;
    onDelete?: (effectId: number) => void;
    onClose?: () => void;
}

export const RippleEffectFormPresentational:
    React.FC<RippleEffectFormProps> = (props) => {
    return (
        <EffectForm {...props}>
            <Grid item xs={4}>
                <label htmlFor="rippleSpeed">Ripple Speed</label>
            </Grid>
            <Grid item xs={8}>
                <Field id="speed" name="speed" type="number"/>
            </Grid>
        </EffectForm>
    );
};
