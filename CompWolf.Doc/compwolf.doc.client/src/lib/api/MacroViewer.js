import { Fragment } from "react";
import { Declaration } from "../CodeComponents";
import FormattedText from "../FormattedText";
import BaseEntityViewer from "./BaseEntityViewer";

export default function MacroViewer({ data }) {

	return (
		<BaseEntityViewer data={data} top={
			<Fragment>
				<big id="Declaration">
					<Declaration>{data.declaration}</Declaration>
				</big>
				<div id="Description">
					<FormattedText>{data.detailedDescription}</FormattedText>
				</div>
			</Fragment>
		}>

		</BaseEntityViewer>
	)
}